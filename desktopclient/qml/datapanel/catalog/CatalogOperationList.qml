import QtQuick 2.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    id : operationsColumn


    Rectangle {
        id : opLabel
        width : 160
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Actions for Selection")
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip{
            target: opLabel
            text : qsTr("List of actions that can be applied to the current selection")
        }

    }
    Rectangle {
        id : operationEditors
        color : uicontext.lightestColor
        width : parent.width
        anchors.top: opLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        ListView{
            id : editorList
            anchors.fill: parent
            model : mastercatalog.catalogOperationEditors

			onModelChanged : {
				editorList.currentIndex = 0
                editorColumn.currentEditor = editorList.model[0]
			}

            highlight: Rectangle{ width : parent ? ( editorList.currentIndex >= 0 ? parent.width : 0) : 0; height : 18; color : Global.selectedColor}
            delegate:  Text {
                x : 4
                text: displayName
                width : editorList.width
                height : 14
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        editorList.currentIndex = index
                        editorColumn.currentEditor = editorList.model[index]
                    }
                }
            }
         }
    }
}

