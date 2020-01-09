import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../.." as Base

Item {
    id : displayEditorDataseries

    property var currentOperation
    property int seriesIndex

    onCurrentOperationChanged: {
	    operation.setSource(currentOperation.qmlUrl,{"operation" : currentOperation, "seriesIndex" : seriesIndex})
     }

	 function currentEditor() {
		return operation.item
	 }

    Rectangle {
        id : operationLabel
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Operation")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : operationLabel
            text : qsTr("Operation that changes the content/organization of a dataseries")
        }
    }

    Rectangle {
        id : operationSeries
        color : uicontext.workbenchBGColor
        border.color: "lightgrey"
        border.width: 1
        anchors.right: parent.right
        anchors.top: operationLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 3
        Loader {
            id : operation
            width : 250
            height : parent.height
        }

    }

}



