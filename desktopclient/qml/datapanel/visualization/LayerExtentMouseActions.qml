import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

MouseArea {
    id : mapArea
    anchors.fill: parent
    hoverEnabled: true
    property LayerManager layerManager
    property bool zoomToExtents : true
    property bool showInfo : true
    property bool hasPermanence : false
    property bool zoomStarted : false
    property bool panStarted : false
    property int panPrevMouseX : -1
    property int panPrevMouseY : -1
    property string selectiondrawerColor : "basic"
    property string subscription
    property var pStart
    property var pEnd
    signal zoomEnded(string envelope)

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        if ( layerview.tabmodel){
            if (!layerview.tabmodel.selected)
                layerview.tabmodel.selectTab()
        }
        if ( layerManager.zoomInMode || layerManager.zoomOutMode){
			if (!zoomStarted){
				pStart = {x : mouseX + parent.x, y : mouseY + parent.y}
				pEnd = pStart
				setRect()
				zoomRectangle.visible = true
				showInfo = false
			}
            zoomStarted = true
        } else if ( layerManager.panningMode ){
            panStarted = true
            panPrevMouseX = mouseX
            panPrevMouseY = mouseY
            showInfo = false
            cursorShape = Qt.ClosedHandCursor

        }
        if ( showInfo && layerManager.rootLayer.showLayerInfo && !zoomStarted && !panStarted){
          floatrect.enabled = true
          floatrect.opacity = 1
          floatrect.x = mouseX
          floatrect.y = mouseY
          var mposition = mouseX + "|" + mouseY
          floatrect.text = layerManager.rootLayer.layerInfo(mposition)
        }
    }

    function setRect() {
        var x1 = pStart.x;
        var x2 = pEnd.x;
        var y1 = pStart.y;
        var y2 = pEnd.y;
        var dx = x2 - x1;
        var dy = y2 - y1;
        var rFactX = Math.abs(dx / width);
        var rFactY = Math.abs(dy / height);
        var rFact = width / height;
        if (rFactX > rFactY)
            y2 = y1 + Math.round(Math.abs(dx) * Math.sign(dy) / rFact);
        else
            x2 = x1 + Math.round(Math.abs(dy) * Math.sign(dx) * rFact);
        if (Math.abs(x1-x2) < 3) x2 = x1;
        if (Math.abs(y1-y2) < 3) y2 = y1;
        if (x1 > x2) {
            var temp = x1
            x1 = x2
            x2 = temp
        }
        if (y1 > y2) {
            var temp = y1
            y1 = y2
            y2 = temp
        }
        zoomRectangle.x = x1
        zoomRectangle.width = x2 - x1
        zoomRectangle.y = y1
        zoomRectangle.height = y2 - y1
    }

    onPositionChanged: {
        if (zoomStarted){
            pEnd = {x : mouseX + parent.x, y : mouseY + parent.y}
            setRect()
        } else if (panStarted){
            var dX = mouseX - panPrevMouseX
            var dY = mouseY - panPrevMouseY
            panPrevMouseX = mouseX
            panPrevMouseY = mouseY
            var cbZoom = manager.rootLayer.zoomEnvelope
            var deltax = (cbZoom.minx - cbZoom.maxx) * dX / width;
            var deltay = (cbZoom.maxy - cbZoom.miny) * dY / height;
            cbZoom.minx += deltax;
            cbZoom.maxx += deltax;
            cbZoom.miny += deltay;
            cbZoom.maxy += deltay;
            var envelope = cbZoom.minx + "," + cbZoom.miny + "," + cbZoom.maxx + "," + cbZoom.maxy
            layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
        }
/*        if (!layerManager.panningMode) {
            cursorShape = Qt.ArrowCursor
        } else {
            if (!panStarted)
                cursorShape = Qt.OpenHandCursor
            else
                cursorShape = Qt.ClosedHandCursor
        }

        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition

        var dX = mouseX - panPrevMouseX
        var dY = mouseY - panPrevMouseY

        if ( (zoomStarted || panStarted) && layerManager.hasSelectionDrawer){
            var position;

            if (!panStarted) { // zooming.....
                position = {currentx: mouseX, currenty:mouseY}
                layerManager.rootLayer.vproperty("selectiondrawer", position)
                //drawer.copyAttribute("selectiondrawer","envelope");
            }
            else {
                var selDrawerEnvelope = drawer.attributeOfDrawer("selectiondrawer","envelope");

                var x1,x2,y1,y2
                var parts = selDrawerEnvelope.split(" ")
                x1 = parseFloat(parts[0])
                y1 = parseFloat(parts[1])
                x2 = parseFloat(parts[2])
                y2 = parseFloat(parts[3])

                var screenpos = {x: mouseX, y: mouseY, z: 0}
                var georefScreenPos = layerManager.screen2Coord(screenpos)

                if (georefScreenPos.x >= x1 && georefScreenPos.x <= x2 && georefScreenPos.y >= y1 && georefScreenPos.y <= y2) {
                    panMapArea(dX, dY)

                    var nx1 = x1 + dX
                    var nx2 = x2 + dX
                    var ny1 = y1 - dY
                    var ny2 = y2 - dY

                    var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
                    viewmanager.newZoomExtent(newenvelope)
                   // layerview.publish( subscription, { envelope: newenvelope, options : false} )
                    panPrevMouseX = mouseX
                    panPrevMouseY = mouseY
                }
            }
            drawer.update()
        }*/
		var mposition = mouseX + "|" + mouseY
		manager.rootLayer.currentCoordinate = mposition
        if ( showInfo && floatrect.opacity > 0){
            floatrect.x = mouseX
            floatrect.y = mouseY
            floatrect.text = manager.rootLayer.layerInfo(mposition)
		//console.debug(floatrect.x, floatrect.y, floatrect.width, floatrect.height, floatrect.text)
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode ){
            pEnd = {x : mouseX + parent.x, y : mouseY + parent.y}
            setRect()
            if (zoomStarted) {
                if (zoomRectangle.width < 3 && zoomRectangle.height < 3) { // case of clicking on the map in zoom mode
                    var cbZoom = manager.rootLayer.zoomEnvelope
                    var posx = cbZoom.minx + (cbZoom.maxx - cbZoom.minx) * zoomRectangle.x / width; // determine click point
                    var posy = cbZoom.maxy - (cbZoom.maxy - cbZoom.miny) * zoomRectangle.y / height;
                    var w = (cbZoom.maxx - cbZoom.minx) / (2.0 * 1.41); // determine new window size
                    var h = (cbZoom.maxy - cbZoom.miny) / (2.0 * 1.41);
                    var envelope = (posx - w) + "," + (posy - h) + "," + (posx + w) + "," + (posy + h) // determine new bounds
                    layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
                } else if (zoomRectangle.width >= 3 && zoomRectangle.height >= 3) {
		            var minPos = {x: zoomRectangle.x - parent.x, y: zoomRectangle.y - parent.y, z: 0}
			        var maxPos = {x: zoomRectangle.width + zoomRectangle.x - parent.x, y: zoomRectangle.height + zoomRectangle.y - parent.y, z: 0}
			        var minCoord = manager.rootLayer.screen2Coord(minPos)
                    var maxCoord = manager.rootLayer.screen2Coord(maxPos)
			        var envelope = minCoord.x + "," + minCoord.y + "," + maxCoord.x + "," + maxCoord.y
                    layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
                } // else if zoomRectangle is a horizontal or a vertical strip, do nothing
            }
			zoomRectangle.disable()
            zoomStarted = false
			showInfo = true
			layerManager.refresh()
            var enablePanAndZoomOut = layerview.manager.rootLayer.scrollInfo.xsizeperc < 1.0 || layerview.manager.rootLayer.scrollInfo.ysizeperc < 1.0
            layerview.maptools.panButton.enabled = enablePanAndZoomOut
            layerview.maptools.zoomoutButton.enabled = enablePanAndZoomOut
        } else if ( layerManager.zoomOutMode ){
            pEnd = {x : mouseX + parent.x, y : mouseY + parent.y}
            setRect()
            if (zoomStarted) {
                if (zoomRectangle.width < 3 && zoomRectangle.height < 3) { // case of clicking on the map in zoom mode
                    var cbZoom = manager.rootLayer.zoomEnvelope
                    var posx = cbZoom.minx + (cbZoom.maxx - cbZoom.minx) * zoomRectangle.x / width; // determine click point
                    var posy = cbZoom.maxy - (cbZoom.maxy - cbZoom.miny) * zoomRectangle.y / height;
                    var w = (cbZoom.maxx - cbZoom.minx) * 1.41 / 2.0; // determine new window size
                    var h = (cbZoom.maxy - cbZoom.miny) * 1.41 / 2.0;
                    var envelope = (posx - w) + "," + (posy - h) + "," + (posx + w) + "," + (posy + h) // determine new bounds
                    layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
                } else if (zoomRectangle.width >= 3 && zoomRectangle.height >= 3) {
                    var top = zoomRectangle.y - parent.y
                    var bottom = zoomRectangle.y + zoomRectangle.height - parent.y
                    var left = zoomRectangle.x - parent.x
                    var right = zoomRectangle.x + zoomRectangle.width - parent.x
                    top = - top;
                    bottom = height + (height - bottom);
                    left = - left;
                    right = width + (width - right);
                    var minPos = {x: left, y: top, z: 0}
                    var maxPos = {x: right, y: bottom, z: 0}
                    var minCoord = manager.rootLayer.screen2Coord(minPos)
                    var maxCoord = manager.rootLayer.screen2Coord(maxPos)
                    var envelope = minCoord.x + "," + minCoord.y + "," + maxCoord.x + "," + maxCoord.y
                    layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
                } // else if zoomRectangle is a horizontal or a vertical strip, do nothing
            }
            zoomRectangle.disable()
            zoomStarted = false
            showInfo = true
            layerManager.refresh()
            var enablePanAndZoomOut = layerview.manager.rootLayer.scrollInfo.xsizeperc < 1.0 || layerview.manager.rootLayer.scrollInfo.ysizeperc < 1.0
            layerview.maptools.panButton.enabled = enablePanAndZoomOut
            layerview.maptools.zoomoutButton.enabled = enablePanAndZoomOut
            layerview.maptools.zoomoutButton.checked = enablePanAndZoomOut
            layerview.maptools.normalButton.checked = !enablePanAndZoomOut
            layerManager.zoomOutMode = enablePanAndZoomOut
        } else if ( layerManager.panningMode ){
            if (panStarted){
                var dX = mouseX - panPrevMouseX
                var dY = mouseY - panPrevMouseY
                panPrevMouseX = mouseX
                panPrevMouseY = mouseY
                var cbZoom = manager.rootLayer.zoomEnvelope
                var deltax = (cbZoom.minx - cbZoom.maxx) * dX / width;
                var deltay = (cbZoom.maxy - cbZoom.miny) * dY / height;
                cbZoom.minx += deltax;
                cbZoom.maxx += deltax;
                cbZoom.miny += deltay;
                cbZoom.maxy += deltay;
                var envelope = cbZoom.minx + "," + cbZoom.miny + "," + cbZoom.maxx + "," + cbZoom.maxy
                layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
            }
            panStarted = false
            panPrevMouseX = -1
            panPrevMouseY = -1
            showInfo = true
            cursorShape = Qt.ArrowCursor
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
    onWheel: {
        if ( manager){
            var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
            Global.calcZoomOutEnvelope(envelope, layers, viewmanager, wheel.angleDelta.y < 0 ? 0.9 : -0.2 )
        }
    }
}


