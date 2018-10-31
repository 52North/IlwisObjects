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
                zoominButton.checked = zoominButton.checked ? false : true
                worldmapcontainer.manager.zoomInMode = zoominButton.checked
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                var envelope = manager.rootLayer.zoomEnvelope;
                var zoomposition = {x: 0.5, y: 0.5};
                envelope = Global.calcZoomOutEnvelope(envelope, zoomposition, worldmapcontainer.manager,0.707);
                worldmap.newExtent(envelope.minx + " " + envelope.miny + " " + envelope.maxx + " " + envelope.maxy);
            }
        }
    }
	Action {
        id : panningClicked
		}

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        MapExtentButton{
            id : normalButton
            icon : "full_map_a.png"
            onClicked: {
                worldmap.addCommand("setviewextent("+ worldmap.viewid + ",entiremap)");
                worldmap.update()
                if ( currentCatalog)
                    currentCatalog.filter("spatial","")

				panButton1.enabled = false
                panButton1.checked = false
                zoomoutButton1.enabled = false
                zoomoutButton1.checked = false
                if (!zoominButton.checked) {
                     normalButton.checked = true
                }
                toolbarClicked()
            }
        }

        MapExtentButton{
            id : zoominButton1
            icon : checked ? "zoom_a.png" : "zoom_i.png"
            checkable: true
            checked: false
            action : zoomClicked

			onClicked: {
                checked = !checked
                normalButton.checked = !checked
                zoomoutButton1.checked = false
                panButton1.checked = false
                toolbarClicked()

            }
        }
        MapExtentButton{
            id : zoomoutButton1
            icon : zoomoutButton1.enabled ? (zoomoutButton1.checked ? "zoom_out_a.png" : "zoom_out_i.png") : "zoom_out_i.png"
            action : zoomOutClicked

			onClicked: {
               if ( enabled){
                    checked = !checked
                    normalButton.checked = !checked
                    zoominButton1.checked = false
                    panButton1.checked = false
                    toolbarClicked()
                }
            }
        }
		MapExtentButton{
            id : panButton1
			checkable: true
            icon : panButton1.enabled ? (panButton1.checked ? "pan_a.png" : "pan_i.png") : "pan_i.png"
            action : panningClicked
			onClicked: {
               if ( enabled){
                    checked = !checked
                    normalButton.checked = !checked
                    zoominButton1.checked = false
                    zoomoutButton1.checked = false
                    toolbarClicked()
                }

            }
        }
    }
}


