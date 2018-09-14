import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.3

import Bear.Editor 1.0
import "qrc:/qml" as BearQml

Page {
	id: bear

	anchors.fill: parent

	property FileDialog fileDialog
	property alias projectHandler: project

	Action {
		id: openaction
		text: "Open"
		shortcut: "Ctrl+O"
		onTriggered: fileDialog.visible = true
	}

	Action {
		id: saveFileAction
		text: "Save"
		shortcut: "Ctrl+S"
		onTriggered: document.saveFile(document.fileUrl)
	}

	Action {
		id: cutAction
		text: "Cut"
		shortcut: "Ctrl+X"
		onTriggered: textEdit.cut()
	}

	Action {
		id: copyAction
		text: "Copy"
		shortcut: "Ctrl+C"
		onTriggered: textEdit.copy()
	}

	Action {
		id: pasteAction
		text: "Paste"
		shortcut: "Ctrl+V"
		onTriggered: textEdit.paste()
	}

	header: Item {
		anchors.left: parent.left
		width: parent.width
		height: childrenRect.height

		MenuBar {
			id: menuBar

			anchors.left: parent.left
			anchors.right: parent.right

			Menu {
				title: "File"
				MenuItem { text: "Close" }
				MenuItem { action: saveFileAction }

				MenuSeparator { }

				MenuItem { text: "Settings" }
				MenuItem { text: "Exit"; onClicked: Qt.quit() }
			}

			Menu {
				title: "Project"
				MenuItem { action: openaction }
				MenuItem { text: "Close" }
			}

			Menu {
				title: "&Edit"
				MenuItem { action: cutAction }
				MenuItem { action: copyAction }
				MenuItem { action: pasteAction }
			}
		}

		ToolBar {
			anchors.top: menuBar.bottom
			anchors.left: parent.left
			width: parent.width

			RowLayout {
				anchors.fill: parent

				RowLayout {
					Label {
						width: 100
						leftPadding: 10
						rightPadding: 10
						text: qsTr("Module")
					}

					ComboBox {
						id: moduleBox

						model: ["Untitled"]
					}

					Label {
						width: 100
						leftPadding: 10
						rightPadding: 10
						text: qsTr("Startup Function")
					}

					ComboBox {
						id: functionBox

						model: ["main", "f", "g"]
					}
				}

				ToolButton {
					anchors.right: parent.right

					leftPadding: 30
					rightPadding: 30

					text: qsTr("Execute")

					onClicked: {
						console.log("EXECUTE " + moduleBox.currentText + ":" + functionBox.currentText)
					}
				}
			}
		}
	}

	footer: ToolBar {
	}

	Item {
		//anchors.top: header.bottom
		anchors.topMargin: header.height
		anchors.left: parent.left

		width: parent.width
		height: parent.height
		id: editor

		Text {
			id: moduleListHeader

			width: 200

			horizontalAlignment: Text.AlignHCenter

			topPadding: 10
			text: "Modules"
			color: "#aaaaaa"
			font.pointSize: 16
		}

		ScrollView {
			id: moduleListRoot

			anchors.top: moduleListHeader.bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left

			width: 200
			height: parent.height

			Item {
				id: moduleList

				anchors.fill: parent
				width: parent.width

				ColumnLayout {
					anchors.fill: parent
					height: moduleListRoot.height

					Text {
						id: moduleHeader
						objectName: "moduleHeader"

						color: "#aaaaaa"
						topPadding: 10
						leftPadding: 20
						text: "Untitled"
						font.pointSize: 14

						Component.onCompleted: text = "↓  " + text
					}

					ListView {
						objectName: "sourceList"

						width: 180;
						height: moduleListRoot.height;

						delegate: MouseArea {
							acceptedButtons: Qt.LeftButton | Qt.RightButton

							property alias text: textInner.text

							onClicked: {
								if(mouse.button & Qt.RightButton) {
								}
								else{
									document.fileUrl = project.dirUrl + "/" + modelData;
								}
							}

							width: textInner.width
							height: textInner.height

							Text{
								id: textInner
								color: "#aaaaaa"
								leftPadding: 40
								text: modelData
								font.pointSize: 12
							}
						}
					}
				}
			}
		}

		TabBar {
			id: bar

			anchors.left: moduleListRoot.right
			anchors.top: parent.top

			TabButton {
				text: "Untitled"
			}
		}

		ScrollView {
			id: scrollView

			anchors.top: bar.bottom
			anchors.left: moduleListRoot.right
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			contentItem: textEdit

			clip: true
			focus: true

			width: parent.width

			contentHeight: textEdit.height

			Rectangle {
				id: lineNumberBackground

				color: Qt.rgba(0,0,0,0)

				anchors.left: parent.left
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				width: 35
				height: parent.height

				TextArea {
					id: lineNumbers
					objectName: "lineNumbers"

					topPadding: 10

					property int number: 0

					background: Rectangle {
						color: Qt.rgba(0,0,0,0);
					}

					readOnly: true

					width: parent.width
					height: textEdit.height

					font.family: "Monoid"
					font.pointSize: 10.0
					color: "#9d9d9d"
					textFormat: TextEdit.PlainText
				}
			}

			TextArea {
				id: textEdit
				objectName: "textEdit"

				//tabStopDistance: fontMetrics.averageCharacterWidth() * 4

				background: Rectangle {
					color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
				}

				anchors.left: lineNumberBackground.right

				width: parent.width

				selectByMouse: true
				//wrapMode: wrap

				topPadding: 10
				persistentSelection: true
				activeFocusOnPress: true
				focus: true

				font.family: "Monoid"
				font.pointSize: 10.0
				color: "lightgray"
				textFormat: TextEdit.PlainText
			}
		}

		ProjectHandler {
			id: project
			objectName: "project"
		}

		DocumentHandler {
			id: document
			objectName: "document"
			textDocument: textEdit.textDocument
		}
	}
}
