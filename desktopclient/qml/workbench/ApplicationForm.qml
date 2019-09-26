import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0
import "../Global.js" as Global

Rectangle {
    property color background1 : "white"
    property string formQML : ""
    property var formComponent : "" 
    property var currentAppForm : null
    property string formTitle
    property string operationId : "id"
    property string operationUrl : "url"
    property bool showTitle: true

    signal formResultChanged();

    id : applicationFormFrame
    width : parent.width
    height : parent.height
    radius : 10
    color : background1

    function doExecute(operationid, runparms) {
        if ( currentAppForm != null) {
            operations.executeoperation(operationid,currentAppForm.formresult, runparms)
        }
    }

    onFormQMLChanged: {
        if ( currentAppForm != null){
            currentAppForm.destroy(0)
        }

        if ( formQML.length !== 0) {
            background1 = formTitle == "" ? "transparent" : "white"
            currentAppForm = Qt.createQmlObject(formQML,
                applicationArea, "autoform1");
		   applicationArea.visible = true
		   formLoader.visible = false
           height = currentAppForm.childrenRect.height + 60
        }
    }

    onFormComponentChanged: {
		if ( formComponent != "" ){
			formLoader.visible = true
			applicationArea.visible = false
			formLoader.source = formComponent	
		}
    }

    Column {
        anchors.fill: parent
        spacing : 3
        Rectangle {
            id : title
            width: parent.width
            height : formTitle != "" && showTitle ? 25 : 0
            opacity : formTitle != "" && showTitle ? 1 : 0
            color : uicontext.paleColor
            smooth : true
            Text  {
                text : formTitle
                font.pointSize: 11
                x : 5
            }
        }
		Item {
			width : parent.width
			height : parent.height - applicationFormFrame.height - 20
			Rectangle {
				id : applicationArea
				property string exprparameters
				width :visible ? parent.width : 0
				height : visible ? parent.height : 0
				visible : true
			}
			Loader {
				id : formLoader
				visible : false
				width : visible ? parent.width : 0
				height : visible && item ? item.height + 60 : 0
				onLoaded: {
					applicationFormFrame.height = item.height + 60
					currentAppForm = item
				}
			}
		}
    }

    Connections {
        target: currentAppForm
        onFormresultChanged: {
            formResultChanged();
        }
    }
}
