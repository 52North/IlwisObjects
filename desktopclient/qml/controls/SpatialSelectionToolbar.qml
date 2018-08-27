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

    property alias zoomoutButton: zoomoutButton
    property alias zoominButton: zoominButton

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
                var envelope = manager.rootLayer.zoomEnvelope
                var zoomposition = {x: 0.5, y: 0.5};
                Global.calcZoomOutEnvelope(envelope, zoomposition, worldmap, worldmapcontainer.manager,0.707)
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        MapExtentButton{
            id : panButton1
            icon : "entiremap20.png"
            onClicked: {
                worldmap.addCommand("setviewextent("+ worldmap.viewid + ",entiremap)");
                worldmap.update()
                if ( currentCatalog)
                    currentCatalog.filter("spatial","")
            }
        }

        MapExtentButton{
            id : zoominButton
            icon : checked ? "zoomin20A.png" : "zoomin20.png"
            checkable: true
            checked: false
            action : zoomClicked
        }
        MapExtentButton{
            id : zoomoutButton
            icon :"zoomout20.png"
            action : zoomOutClicked
        }
    }
}


