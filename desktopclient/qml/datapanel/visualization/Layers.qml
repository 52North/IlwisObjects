import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Item {
    id : layerContainer
    objectName: "layerContainer_ui"  + uicontext.uniqueName()
    property alias layerContainerId : layerContainer
    property variant subscription
    signal zoomEnded(string envelope) 

    property bool reportExtentsBack : false

    MapArea{
        anchors.fill : parent
        id : maparea

        onWidthChanged: {
            publishDrawerPixelarea()
        }

        onHeightChanged: {
            publishDrawerPixelarea()
        }
		Component.onCompleted : {
			layerview.manager.viewArea(maparea)
		}
    }

    MapScrollers {
        anchors.fill: parent
        id : mapScrollers
    }

    function rootLayer() {
        maparea.rootLayer
    }

    function drawer(){
        return maparea.manager
    }

    function maparea(){
        return maparea
    }

    function transfer(mappane){
        maparea.transfer(mappane)
    }

    function addDataSource(filter, sourceName, sourceType){
        var filter2 = filter
        if ( filter.indexOf("=") !== -1){
            filter = "\"" + filter + "\""
        }
		var cmd = "adddrawer(" + manager.viewid + ",\"\"," + filter + "," + sourceType + ",true)"
        layerview.manager.addCommand(cmd)
        layerview.manager.refresh()
        viewmanager.addDataSource(filter2, sourceName, sourceType)
        return layerview.manager.viewid
    }

    function newExtent(ext){
        layerview.manager.addCommand("setviewextent("+ layerview.manager.viewid + "," + ext + ")");
        layerview.manager.refresh()
    }

    function publishDrawerPixelarea() {
        if ( !layerview.manager)
            return

        var pixelarea = layerview.manager.rootLayer.vproperty("rootdrawer","pixelarea");
        if (pixelarea && pixelarea !== "") {
            var pixelareaparts = pixelarea.split(" ");

            var w = pixelareaparts[0];
            var h = pixelareaparts[1];

            var minPos = {x: 0, y: h, z: 0}
            var maxPos = {x: w, y: 0, z: 0}

            var minCoord = manager.screen2Coord(minPos)
            var maxCoord = manager.screen2Coord(maxPos)
            var shownEnvelope = minCoord.x + " " + minCoord.y + " " + maxCoord.x + " " + maxCoord.y

            // when multiple layers are present, this is needed while resizing the maparea
            newExtent(shownEnvelope);

            layerview.publish(layerview.selectiondrawertopicoverview, { envelope : shownEnvelope, options : false })
        }
    }

    function clbkSynchronizeEnded(){
        if (reportExtentsBack) {
            publishDrawerPixelarea()
            reportExtentsBack = false
        }

        // todo: fix this
      /*var coverageenvelope = layerview.manager.attributeOfDrawer("rootdrawer","coverageenvelope");

        var parts = coverageenvelope.split(" ")
        var x1 = parseFloat(parts[0])
        var y1 = parseFloat(parts[1])
        var x2 = parseFloat(parts[2])
        var y2 = parseFloat(parts[3])
        // need to do this, so that the overview map is happy when layers are dropped on the main map
        layerview.manager.viewEnvelope = {minx: x1, miny: y1, maxx: x2, maxy: y2}
        */
    }



}

