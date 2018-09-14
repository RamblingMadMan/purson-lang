import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.4

import "qrc:/qml"

ApplicationWindow {
	id: appWindow
	objectName: "appWindow"

	visible: true
	minimumWidth: 640
	minimumHeight: 480
	title: qsTr("Bear")

	Material.theme: Material.Dark
	Material.primary: Material.Grey
	Material.accent: Material.DeepOrange

	ProjectWindow {
		id: project
		fileDialog: openDialog
	}

	FileDialog {
		id: openDialog
		objectName: "openDialog"

		title: "Choose a project directory"
		folder: shortcuts.home

		onAccepted: {
			project.projectHandler.openProject(openDialog.folder)
		}
	}
}
