#include "antilatency_tracker_alt_dialog.h"
#include "api/plugin-api.hpp"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>

namespace Antilatency {
	AltDialog::AltDialog() : tracker(nullptr) {
		ui.setupUi( this );

		connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
		connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));

        tie_setting(_settings.ExtrapolationTime, ui.ExtrapolationSpinBox);
        tie_setting(_settings.UsePlacementCorrection, ui.UsePlacementCheckBox);
	}

	void AltDialog::doOK() {
        _settings.b->save();
		close();
	}

	void AltDialog::doCancel() {
		close();
	}

    OPENTRACK_DECLARE_TRACKER(AltTracker, AltDialog, AltMetadata)
}
