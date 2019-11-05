#pragma once

#include <cmath>
#include "api/plugin-api.hpp"
#include "antilatency_tracker_alt_settings.h"

#include <Antilatency.DeviceNetwork.h>
#include <Antilatency.Alt.Tracking.h>
#include <Antilatency.InterfaceContract.LibraryLoader.h>

namespace Antilatency {
	class AltTracker : public ITracker {
	public:
        AltTracker();
		~AltTracker();

        module_status start_tracker(QFrame *);
		void data(double* data);
		bool center();

	private:
		Antilatency::Alt::Tracking::ICotask _trackingCotask = nullptr;
		Antilatency::DeviceNetwork::INetwork _network = nullptr;
		Antilatency::Alt::Tracking::IEnvironment _environment = nullptr;
		Antilatency::Alt::Tracking::ILibrary _trackingLibrary = nullptr;
		Antilatency::StorageClient::ILibrary _storageClientLibrary = nullptr;
		Antilatency::DeviceNetwork::ILibrary _adnLibrary = nullptr;

        AltTrackerSettings _settings;
		uint32_t _updateId = 0;
        Math::floatP3Q _placement = {};
        float _extrapolationTime = 0.0f;

		Antilatency::DeviceNetwork::NodeHandle GetTrackingNode();
		void StartTrackingTask(Antilatency::DeviceNetwork::NodeHandle node);
		void StopTrackingTask();
		Antilatency::Math::floatP3Q GetTrackingPose();
	};
}
