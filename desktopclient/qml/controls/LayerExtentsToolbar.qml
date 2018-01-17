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
    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        spacing : 2
        MapExtentButton{
            icon  :"entiremap20.png"
            id : entireMap
            action : entireClicked
            onClicked: {
                normalButton.checked = true
                zoominButton.checked = false
                zoomoutButton.checked = false
                panButton.checked = false
            }
            Controls.ToolTip{
                target: entireMap
                text : qsTr("Expands the view to the maximal spatial extent as defined by the layers")
            }
        }

        MapExtentButton{
            id : panButton
            icon : panButton.checked ? "pan20A.png" : "pan20.png"
            action : panningClicked
            checkable: true
            checked: false
            onClicked: {
                checked = !checked
                normalButton.checked = !checked
                zoominButton.checked = false
                zoomoutButton.checked = false

            }
            Controls.ToolTip{
                target: panButton
                text : qsTr("Triggers a state so that a mouse movement, while keeping the mouse button pressed,\n moves visibility window in the map panel ")
            }
        }

        MapExtentButton{
            id : zoominButton
            icon : zoominButton.checked ? "zoomin20A.png" : "zoomin20.png"
            action : zoomClicked
            checkable: true
            checked: false
            onClicked: {
                checked = !checked
                normalButton.checked = !checked
                zoomoutButton.checked = false
                panButton.checked = false
            }
            Controls.ToolTip{
                target: zoominButton
                text : qsTr("Trigger a state so that a rectangle can be drawn for zooming in")
            }
        }
        MapExtentButton{
            id : zoomoutButton
            icon : zoomoutButton.checked ? "zoomout20A.png" : "zoomout20.png"
            action : zoomOutClicked
            checkable: true
            checked: false
            onClicked: {
                checked = !checked
                normalButton.checked = !checked
                zoominButton.checked = false
                panButton.checked = false
            }
            Controls.ToolTip{
                target: zoomoutButton
                text : qsTr("Trigger a state so that a rectangle can be drawn for zooming out")
            }
        }
        MapExtentButton{
            id : normalButton
            icon : normalButton.checked ? "arrow20A.png" : "arrow20.png"
            action : normalClicked
            checkable: true
            checked: true
            onClicked: {
                checked = true
                zoominButton.checked = false
                zoomoutButton.checked = false
                panButton.checked = false
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


