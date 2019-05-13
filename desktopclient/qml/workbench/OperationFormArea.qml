import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import QtQuick.Controls 2.3 as QC2
import OperationModel 1.0
import MasterCatalogModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

Rectangle {
    id : applicationForm
    height : 0
    x : parent.x + 5
	state : "minimized"
	property OperationModel operationmd


    function newForm(metaid, title, url){
        operationid = metaid
        var form= formbuilder.index2Form(metaid, true)
		operationmd = operations.operation(metaid)
        appF().formQML = form
        appF().formTitle = title
        appF().opacity = 1
    }

	TabView {
		id : tabs
		width : parent.width
		height : parent.height
		Tab {
			title : qsTr("Form")
			Column {
				id : holder
				width : parent.width
				height : parent.height
				function frame() {
					return appFrame
				}
				ApplicationForm{
					id : appFrame
					 width : parent.width
					height : parent.height - 50 < 0 ?  0 : parent.height - 50
					opacity : 0
				}
				Button{
					y : parent.height - 25
					width : 60
					text : "execute"
					height : 25
					x : parent.width - 60
					onClicked: appFrame.doExecute(operationid, false)
				}
			}
		}
		Tab { 
			title : "Descscription"
			Rectangle {
			width : parent.width
				height : parent.height


			}
			ScrollView {
				width : parent.width
				height : parent.height
				QC2.TextArea {
					textFormat : TextEdit.RichText
					text : operationmd ? operationmd.fullDescription() : ""
				}
			}
		}
	}

	function appF() {
		return tabs.getTab(0).item.frame()
	}
    states: [
        State { name: "maximized"

            PropertyChanges {
                target: applicationForm
                height : appF() ? appF().height + 25 : 370
                opacity : 1

            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: applicationForm
                height : 0
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]
}

