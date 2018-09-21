import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
//import "./propertyeditors" as LayerManagement
import "../controls" as Controls
import "../Global.js" as Global

ToolBar{
    id : maptools
    width : parent.width
    height : 35
    property alias normalButton: normalButton
    property alias panButton: panButton
    property alias zoomoutButton: zoomoutButton
    property alias zoominButton : zoominButton
	property bool extraZoomOptions : false
   
    signal toolbarClicked()

    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        spacing : 2
        MapExtentButton{
            icon  :"entiremap20.png"
            id : entireMap
            action : entireClicked
            onClicked: {
               if ( enabled){
                    panButton.enabled = false
                    panButton.checked = false
                    zoomoutButton.enabled = false
                    zoomoutButton.checked = false
                    if (!zoominButton.checked) {
                        normalButton.checked = true
                    }
                    toolbarClicked()
                }
            }
            Controls.ToolTip{
                target: entireMap
                text : qsTr("Expands the view to the maximal spatial extent as defined by the layers")
            }
        }

        MapExtentButton{
            id : panButton
            icon : panButton.enabled ? (panButton.checked ? "pan20A.png" : "pan20.png") : "pan20B.png"
            action : panningClicked
            checkable: true
            checked: false
            onClicked: {
               if ( enabled){
                    checked = !checked
                    normalButton.checked = !checked
                    zoominButton.checked = false
                    zoomoutButton.checked = false
                    toolbarClicked()
                }

            }
            Controls.ToolTip{
                target: panButton
                text : qsTr("Triggers a state so that a mouse movement, while keeping the mouse button pressed,\n moves visibility window in the map panel ")
            }
            Component.onCompleted: {
                enabled = false
            }
        }

        MapExtentButton{
            id : zoominButton
            icon : zoominButton.checked ? "zoomin20A.png" : "zoomin20.png"
            action : zoomClicked
            checkable: true
            checked: false
            onClicked: {
            if ( enabled){
                    checked = !checked
                    normalButton.checked = !checked
                    zoomoutButton.checked = false
                    panButton.checked = false
                    toolbarClicked()
                }
            }
            Controls.ToolTip{
                target: zoominButton
                text : qsTr("Trigger a state so that a rectangle can be drawn for zooming in")
            }
        }
		Controls.ToolButton{
		    id : extraZoomOpt
            height : 25
            width : 14
			iconW : width - 4
			iconH : iconW
            iconSource: "../images/darkbluearrow.png"
            onClicked: {
			    extraZoom.layermanager = layerview.activeLayerManager()
                extraZoom.x = x
                extraZoom.toggle()
            }
			//visible : extraZoomOptions
		}
        MapExtentButton{
            id : zoomoutButton
            icon : zoomoutButton.enabled ? (zoomoutButton.checked ? "zoomout20A.png" : "zoomout20.png") : "zoomout20B.png"
            action : zoomOutClicked
            checkable: true
            checked: false

            onClicked: {
               if ( enabled){
                    checked = !checked
                    normalButton.checked = !checked
                    zoominButton.checked = false
                    panButton.checked = false
                    toolbarClicked()
                }
            }
            Controls.ToolTip{
                target: zoomoutButton
                text : qsTr("Trigger a state so that a rectangle can be drawn for zooming out")
            }
            Component.onCompleted: {
                enabled = false
            }
        }
        MapExtentButton{
            id : normalButton
            icon : normalButton.checked ? "arrow20A.png" : "arrow20.png"
            action : normalClicked
            checkable: true
            checked: true
            onClicked: {
               if ( enabled){
                    checked = true
                    zoominButton.checked = false
                    zoomoutButton.checked = false
                    panButton.checked = false
                    toolbarClicked()
                }
            }
            Controls.ToolTip{
                target: normalButton
                text : qsTr("Set mouse function to Normal")
            }
            Component.onCompleted: {
                checked = true
            }
        }
    }


}


