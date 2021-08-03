#include "antilatency_tracker_alt.h"
#include "api/plugin-api.hpp"
#include "compat/math.hpp"
#include <QMutexLocker>

#include "Antilatency.Math.float3.h"
#include "Antilatency.Math.floatQ.h"

namespace Antilatency {
	AltTracker::AltTracker() {
        //Load Antilatency Device Network library to work with Antilatency devices
		_adnLibrary = Antilatency::InterfaceContract::getLibraryInterface<Antilatency::DeviceNetwork::ILibrary>("AntilatencyDeviceNetwork");
        if (_adnLibrary == nullptr) {
            //Failed to load Antilatency Device Network library
            return;
        }

        //Load Antilatency Alt Tracking library
		_trackingLibrary = Antilatency::InterfaceContract::getLibraryInterface<Antilatency::Alt::Tracking::ILibrary>("AntilatencyAltTracking");
        if (_trackingLibrary == nullptr) {
            //Failed to load Antilatency Alt Tracking library
            return;
        }

        //Load Antilatency Storage Client library to read environment (tracking zone markers pattern) from Antilatency Service
		_storageClientLibrary = Antilatency::InterfaceContract::getLibraryInterface<Antilatency::StorageClient::ILibrary>("AntilatencyStorageClient");
        if (_storageClientLibrary == nullptr) {
            //Failed to load Antilatency Storage Client library
            return;
        }

        // Load Antilatency Alt Environment Selector library
        _environmentSelectorLibrary =
            Antilatency::InterfaceContract::getLibraryInterface<Antilatency::Alt::Environment::Selector::ILibrary>(
                "AntilatencyAltEnvironmentSelector");
        if (_storageClientLibrary == nullptr) {
            // Failed to load Antilatency Alt Environment Selector library
            return;
        }

        // Create a device network filter and then create a network using that filter.
        Antilatency::DeviceNetwork::IDeviceFilter filter = _adnLibrary.createFilter();
        filter.addUsbDevice(Antilatency::DeviceNetwork::Constants::AllUsbDevices);

        // Create the network to work with Antilatency devices
        _network = _adnLibrary.createNetwork(filter);
        if (_network == nullptr) {
            //Failed to create device network
            return;
        }

        _trackingCotaskConstructor = _trackingLibrary.createTrackingCotaskConstructor();
        if (_trackingCotaskConstructor == nullptr) {
            // Failed to create tracking cotask constructor
            return;
        }
	}

	AltTracker::~AltTracker() {
		if (_trackingCotask != nullptr) {
			_trackingCotask = {};
		}
	}

    module_status AltTracker::start_tracker(QFrame *) {
        if (_adnLibrary == nullptr) {
            return error("Antilatency Device Network library is null");;
        }

        if (_trackingLibrary == nullptr) {
            return error("Antilatency Alt Tracking library is null");
        }

        if (_storageClientLibrary == nullptr) {
            return error("Failed to load Antilatency Storage Client library");
        }

        if (_environmentSelectorLibrary == nullptr) {
            return error("Failed to load Antilatency Alt Environment Selector library");
        }

        if (_network == nullptr) {
            return error("Failed to create device network");
        }

        if (_trackingCotask == nullptr) {
            return error("Failed to create tracking cotask constructor");
        }

        return status_ok();
    }

    // Find first idle tracking supported node
	Antilatency::DeviceNetwork::NodeHandle AltTracker::GetTrackingNode() {
		auto result = Antilatency::DeviceNetwork::NodeHandle::Null;

		if (_trackingLibrary == nullptr) {
			return result;
		}

        if (_adnLibrary == nullptr) {
            return result;
        }

		auto nodes = _trackingCotaskConstructor.findSupportedNodes(_network);
		if (!nodes.empty()) {
			for (auto node : nodes) {
				if (_network.nodeGetStatus(node) == Antilatency::DeviceNetwork::NodeStatus::Idle) {
					result = node;
					break;
				}
			}
		}

		return result;
	}

	void AltTracker::StartTrackingTask(Antilatency::DeviceNetwork::NodeHandle node) {
        if (node == Antilatency::DeviceNetwork::NodeHandle::Null) {
            return;
        }

		if (_trackingLibrary == nullptr || _network == nullptr || _storageClientLibrary == nullptr) {
			return;
		}

		auto localStorage = _storageClientLibrary.getLocalStorage();
		if (localStorage == nullptr) {
            //Failed to get local storage, seems like Antilatency Service is not installed
			return;
		}

		auto environmentCode = localStorage.read("environment", "default");
        if (environmentCode.empty()) {
            //Default environment is null, run Antilatency Service and set the default environment
            return;
        }

		_environment = _environmentSelectorLibrary.createEnvironment(environmentCode);

		if (_environment == nullptr) {
            //Failed to create environment from code
			return;
		}

        if (_settings.UsePlacementCorrection) {
            auto placementCode = localStorage.read("placement", "default");
            if (!placementCode.empty()) {
                _placement = _trackingLibrary.createPlacement(placementCode);
            }
        } else {
            _placement.position = { 0.0f, 0.0f, 0.0f };
            _placement.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
        }

        //Start tracking on node
		_trackingCotask = _trackingCotaskConstructor.startTask(_network, node, _environment);
	}

	void AltTracker::StopTrackingTask() {
		_trackingCotask = {};
	}

	Antilatency::Math::floatP3Q AltTracker::GetTrackingPose() {
        //Check if there are any updates in the device network
		auto updateId = _network.getUpdateId();

		if (updateId != _updateId) {
			if (_trackingCotask != nullptr && _trackingCotask.isTaskFinished()) {
                //Seems like Alt tracker is disconnected. Stop tracking task
				StopTrackingTask();
			}

            if (_trackingCotask == nullptr) {
                auto node = GetTrackingNode();
                if (node != Antilatency::DeviceNetwork::NodeHandle::Null) {
                    if (_network.nodeGetStatus(node) == Antilatency::DeviceNetwork::NodeStatus::Idle) {
                        //There is an idle Alt tracker, so we can start tracking task on it
                        StartTrackingTask(node);
                    }
                }
            }

			_updateId = updateId;
		}

		if (_trackingCotask != nullptr) {
			auto state = _trackingCotask.getExtrapolatedState(_placement, _extrapolationTime);
			if (state.stability.stage == Antilatency::Alt::Tracking::Stage::InertialDataInitialization) {
                //At the inertial data initialization state tracking data is not valid, return identity pose
				state.pose.position = {};
				state.pose.rotation = {};

				return state.pose;
			}

            //Alt tracker returns position coordinates in meters
			state.pose.position = state.pose.position * 100.0f;
			return state.pose;
		} else {
            //No tracking task is currently running, just return identity pose
			Antilatency::Math::floatP3Q pose;

			pose.position = {};
			pose.rotation = {};

			return pose;
		}
	}

	void AltTracker::data(double *data) {
        //Get tracker pose. If tracker is disconnected, the identity pose (position = {0, 0, 0} and rotation = {0, 0, 0} will be returned)
		auto pose = GetTrackingPose();

        //Convert pose rotation quaternion to Euler angles
		double pitch, yaw, roll;
        ToEulerAngles(pose.rotation, yaw, pitch, roll);

		data[Yaw] = yaw;
		data[Pitch] = pitch;
		data[Roll] = roll;
		data[TX] = static_cast<double>(pose.position.x);
		data[TY] = static_cast<double>(pose.position.y);
		data[TZ] = static_cast<double>(pose.position.z);
	}

	bool AltTracker::center() {
		return false;
	}
}
