#pragma once

#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QSettings>
#include <QList>
#include <QFrame>
#include <QStringList>

#include "antilatency_tracker_alt.h"
#include "ui_antilatency_tracker_alt_controls.h"
#include "antilatency_tracker_alt_settings.h"

namespace Antilatency {
	class AltDialog : public ITrackerDialog {
		Q_OBJECT
	public:
		AltDialog();
		void register_tracker(ITracker *) {}
		void unregister_tracker() {}

		Ui::UiAltSettings ui;
		AltTracker* tracker;

    private:
        AltTrackerSettings _settings;

	private slots:
		void doOK();
		void doCancel();
	};

	class AltMetadata : public Metadata {
		Q_OBJECT

		QString name() { return tr("Antilatency Alt tracking"); }
		QIcon icon() { return QIcon(":/resources/alt-logo.png"); }
	};
}
