import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../controls" as Controls
import "../Global.js" as Global
import "../.." as Base

Rectangle {
    height : parent.height
    width : 27
    color : Global.alternatecolor1

    property alias zoomoutButton: zoomoutButton1
    property alias zoominButton: zoominButton1
	property var panButton : panButton1

	signal toolbarClicked()

    Action {
        id : zoomClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                worldmapcontainer.manager.zoomInMode = !worldmapcontainer.manager.zoomInMode
                worldmapcontainer.manager.zoomOutMode = false
                worldmapcontainer.manager.panningMode = false
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                worldmapcontainer.manager.zoomOutMode = !worldmapcontainer.manager.zoomOutMode
                worldmapcontainer.manager.zoomInMode = false
                worldmapcontainer.manager.panningMode = false
            }
        }
    }
	Action {
        id : panningClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                worldmapcontainer.manager.panningMode = !worldmapcontainer.manager.panningMode
                worldmapcontainer.manager.zoomInMode = false
                worldmapcontainer.manager.zoomOutMode = false
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        MapExtentButton{
            id : entireMap
            icon : "full_green.svg"
            pushed: pressed
            onClicked: {
                worldmap.addCommand("setviewextent("+ worldmap.viewid + ",entiremap)");
                worldmap.update()
                if ( currentCatalog)
                    currentCatalog.filter("spatial","")

                panButton1.checked = false
                zoomoutButton1.checked = false
                toolbarClicked()
            }
        }

        MapExtentButton{
            id : zoominButton1
            icon : enabled ? "zoomin_green.svg" : "zoomin_grey.svg"
            pushed : checked
            checkable: true
            checked: false
            action : zoomClicked

			onClicked: {
                checked = !checked
                zoomoutButton1.checked = false
                panButton1.checked = false
                toolbarClicked()
            }
        }
        MapExtentButton{
            id : zoomoutButton1
            icon : zoomoutButton1.enabled ? "zoomout_green.svg" : "zoomout_grey.svg"
            pushed : zoomoutButton1.enabled ? zoomoutButton1.checked : false
            action : zoomOutClicked

			onClicked: {
               if ( enabled){
                    checked = !checked
                    zoominButton1.checked = false
                    panButton1.checked = false
                    toolbarClicked()
                }
            }
        }
		MapExtentButton{
            id : panButton1
			checkable: true
            icon : panButton1.enabled ? "pan_green.svg" : "pan_grey.svg"
            pushed : panButton1.enabled ? panButton1.checked : false
            action : panningClicked
			onClicked: {
               if ( enabled){
                    checked = !checked
                    zoominButton1.checked = false
                    zoomoutButton1.checked = false
                    toolbarClicked()
                }
            }
        }
    }
}
