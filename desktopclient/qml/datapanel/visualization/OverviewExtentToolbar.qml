import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    width : 27
    color : Global.alternatecolor1

    Action {
        id : zoomClicked
        onTriggered : {
            if ( metatdata.manager){
                zoominButton1.checked = zoominButton1.checked ? false : true
                metatdata.manager.zoomInMode = zoominButton1.checked
                panButton1.checked = false
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( manager){
                var envelope = manager.rootLayer.zoomEnvelope;
                var zoomposition = {x: 0.5, y: 0.5};
                envelope = Global.calcZoomOutEnvelope(envelope, zoomposition, viewmanager,0.707);
                layers.newExtent(envelope.minx + " " + envelope.miny + " " + envelope.maxx + " " + envelope.maxy);
            }
        }
    }

    Action {
        id : panningClicked
        onTriggered : {
            if ( manager){
                panButton1.checked = !panButton1.checked
                zoominButton1.checked = false
                manager.panningMode = !manager.panningMode
                manager.zoomInMode = false
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        Controls.MapExtentButton{
            id : panButton1
            checkable: true
            checked: false
            icon : checked ? "pan_a.png" : "pan_i.png"
            action : panningClicked
        }

        Controls.MapExtentButton{
            id : zoominButton1
            icon : checked ? "zoom_out_a.png" : "zoom_out_i.png"
            checkable: true
            checked: false
            action : zoomClicked
        }
        Controls.MapExtentButton{
            id : zoomoutButton1
            icon :"zoomout20.png"
            action : zoomOutClicked
        }
    }
}
