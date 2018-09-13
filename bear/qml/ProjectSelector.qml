import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.3

Item {
	anchors.fill: parent

	ColumnLayout {
		ListView {
			id: recentProjectsView

			width: childrenRect.width
			height: childrenRect.height

			model: ["Main"]
			delegate: Text {
				text: modelData
			}
		}

		RowLayout {
			Button {
				text: "New"
			}

			Button {
				text: "Open"
			}
		}
	}
}
