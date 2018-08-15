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

Column {
    id : column
    property int itemHeight : 20
    property int fSize : 8


    Rectangle {
        id : layersLabel
        width : parent.width -2
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Spatial metadata")
            font.bold : true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().viewid

        labelText : qsTr("Viewer id")

    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? layerview.activeLayerManager().rootLayer.screenCsy.name : ""

        labelText : qsTr("Coordinate System")

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? layerview.activeLayerManager().rootLayer.projectionInfo : ""
        labelText : qsTr("Projection")

    }
    Controls.TextEditLabelPair{
        id : viewenv
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        labelText : qsTr("View Extent")
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope(layerview.activeLayerManager().rootLayer.viewEnvelope, true) : ""

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        labelText :""
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope(layerview.activeLayerManager().rootLayer.viewEnvelope, false) : ""

    }
    Controls.TextEditLabelPair{
        id : zoomEnvelope1
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        labelText : qsTr("Zoom Extent ")
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope(layerview.activeLayerManager().rootLayer.zoomEnvelope, true) : ""

    }
    Controls.TextEditLabelPair{
        id : zoomEnvelope2
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope(layerview.activeLayerManager().rootLayer.zoomEnvelope, false) : ""
    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope2(layerview.activeLayerManager().rootLayer.latlonEnvelope, true) : ""
        labelText : qsTr("Lat/Lon Extent")

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 90
        fontSize: fSize
        content : layerview.activeLayerManager().rootLayer.screenCsy ? column.setEnvelope(layerview.activeLayerManager().rootLayer.latlonEnvelope, false) : ""
    }

    function setEnvelope(env, ismin){
        if ( !env || !("minx" in env))
            return ""

        if ( ismin)
            return env.minx.toFixed(4) + " " + env.miny.toFixed(4)
        else
            return env.maxx.toFixed(4) + " " + env.maxy.toFixed(4)

    }

     function setEnvelope2(env, ismin){
        if ( !env || !("minx" in env))
            return ""
        if ( ismin)
            return env.minx.toFixed(4) + " " + env.miny.toFixed(4)
        else
            return env.maxx.toFixed(4) + " " + env.maxy.toFixed(4)

    }

    function zoomEnvelope(newenvelope){
        var parts = newenvelope.split(" ")
        if ( parts.length >= 4) {
            zoomEnvelope1.content =  parseFloat(parts[0]).toFixed(4) + " " + parseFloat(parts[1]).toFixed(4)
            zoomEnvelope2.content =  parseFloat(parts[2]).toFixed(4) + " " + parseFloat(parts[3]).toFixed(4)
        }
    }
}


