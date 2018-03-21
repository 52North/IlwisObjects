import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../Global.js" as Global

MouseArea {
    id : mapArea
    width : parent.width
    height : parent.height
    hoverEnabled: true
    property LayerManager layerManager
   // property var targetRectangle
    property bool zoomToExtents : true
    property bool showInfo : true
    property bool hasPermanence : false
    property bool zoomStarted : false
    property bool panStarted : false
    property int panPrevMouseX : -1
    property int panPrevMouseY : -1
    property string selectiondrawerColor : "basic"
    property var pStart
    property var pEnd
    signal zoomEnded(string envelope)
    signal setZoomPanButton(bool enablePanAndZoomOut)
    signal checkZoomNormalButton(bool enablePanAndZoomOut)
    signal selectTab()

    FloatingRectangle{
        id : floatrect
    }

    Rectangle {
	    id : targetRectangle
	    x : 0
	    y : 0
	    width : 0
	    height : 0
	    visible : false
	    border.width : 1
	    border.color : "midnightblue"
	    color : "transparent"
	    z : 100

	    function disable() {
		    x = 0
		    y = 0
		    width = 0
		    height = 0
		    visible = false
	    }
	}

	Rectangle {
		anchors.fill : targetRectangle
		color : "grey"
		opacity : 0.2
		z: 101
	}

    onPressed:  {
        selectTab()
        if ( layerManager.zoomInMode || layerManager.zoomOutMode){
			if (!zoomStarted){
				pStart = {x : mouseX, y : mouseY}
				pEnd = pStart
				setRect()
				targetRectangle.visible = true
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
        targetRectangle.x = x1
        targetRectangle.width = x2 - x1
        targetRectangle.y = y1
        targetRectangle.height = y2 - y1
    }

    onPositionChanged: {
        if (zoomStarted){
            pEnd = {x : mouseX, y : mouseY }
            setRect()
        } else if (panStarted){
            var dX = mouseX - panPrevMouseX
            var dY = mouseY - panPrevMouseY
            panPrevMouseX = mouseX
            panPrevMouseY = mouseY
            var cbZoom = layerManager.rootLayer.zoomEnvelope
            var deltax = (cbZoom.minx - cbZoom.maxx) * dX / width;
            var deltay = (cbZoom.maxy - cbZoom.miny) * dY / height;
            cbZoom.minx += deltax;
            cbZoom.maxx += deltax;
            cbZoom.miny += deltay;
            cbZoom.maxy += deltay;
            var envelope = cbZoom.minx + "," + cbZoom.miny + "," + cbZoom.maxx + "," + cbZoom.maxy
            layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
        }
		var mposition = mouseX + "|" + mouseY
		layerManager.rootLayer.currentCoordinate = mposition
        if ( showInfo && floatrect.opacity > 0){
            floatrect.x = mouseX
            floatrect.y = mouseY
            floatrect.text = layerManager.rootLayer.layerInfo(mposition)
		//console.debug(floatrect.x, floatrect.y, floatrect.width, floatrect.height, floatrect.text)
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode ){
            pEnd = {x : mouseX , y : mouseY }
            setRect()
            if (zoomStarted) {
                if (targetRectangle.width < 3 && targetRectangle.height < 3) { // case of clicking on the map in zoom mode
                    var cbZoom = layerManager.rootLayer.zoomEnvelope
                    var posx = cbZoom.minx + (cbZoom.maxx - cbZoom.minx) * targetRectangle.x / width; // determine click point
                    var posy = cbZoom.maxy - (cbZoom.maxy - cbZoom.miny) * targetRectangle.y / height;
                    var w = (cbZoom.maxx - cbZoom.minx) / (2.0 * 1.41); // determine new window size
                    var h = (cbZoom.maxy - cbZoom.miny) / (2.0 * 1.41);
                    var envelope = (posx - w) + "," + (posy - h) + "," + (posx + w) + "," + (posy + h) // determine new bounds
                    layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
                    zoomEnded(envelope)
                } else if (targetRectangle.width >= 3 && targetRectangle.height >= 3) {
		            var minPos = {x: targetRectangle.x  , y: targetRectangle.y , z: 0}
			        var maxPos = {x: targetRectangle.width + targetRectangle.x  , y: targetRectangle.height + targetRectangle.y, z: 0}
			        var minCoord = layerManager.rootLayer.screen2Coord(minPos)
                    var maxCoord = layerManager.rootLayer.screen2Coord(maxPos)
			        var envelope = minCoord.x + "," + minCoord.y + "," + maxCoord.x + "," + maxCoord.y
                    layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
                    zoomEnded(envelope)
                } // else if targetRectangle is a horizontal or a vertical strip, do nothing
            }
			targetRectangle.disable()
            zoomStarted = false
			showInfo = true
			layerManager.refresh()
            var enablePanAndZoomOut = layerManager.rootLayer.scrollInfo.xsizeperc < 1.0 || layerManager.rootLayer.scrollInfo.ysizeperc < 1.0
            setZoomPanButton(enablePanAndZoomOut)
        } else if ( layerManager.zoomOutMode ){
            pEnd = {x : mouseX , y : mouseY}
            setRect()
            if (zoomStarted) {
                if (targetRectangle.width < 3 && targetRectangle.height < 3) { // case of clicking on the map in zoom mode
                    var cbZoom = layerManager.rootLayer.zoomEnvelope
                    var posx = cbZoom.minx + (cbZoom.maxx - cbZoom.minx) * targetRectangle.x / width; // determine click point
                    var posy = cbZoom.maxy - (cbZoom.maxy - cbZoom.miny) * targetRectangle.y / height;
                    var w = (cbZoom.maxx - cbZoom.minx) * 1.41 / 2.0; // determine new window size
                    var h = (cbZoom.maxy - cbZoom.miny) * 1.41 / 2.0;
                    var envelope = (posx - w) + "," + (posy - h) + "," + (posx + w) + "," + (posy + h) // determine new bounds
                    layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
                    zoomEnded(envelope)
                } else if (targetRectangle.width >= 3 && targetRectangle.height >= 3) {
                    var top = targetRectangle.y 
                    var bottom = targetRectangle.y + targetRectangle.height
                    var left = targetRectangle.x
                    var right = targetRectangle.x + targetRectangle.width
                    top = - top;
                    bottom = height + (height - bottom);
                    left = - left;
                    right = width + (width - right);
                    var minPos = {x: left, y: top, z: 0}
                    var maxPos = {x: right, y: bottom, z: 0}
                    var minCoord = layerManager.rootLayer.screen2Coord(minPos)
                    var maxCoord = layerManager.rootLayer.screen2Coord(maxPos)
                    var envelope = minCoord.x + "," + minCoord.y + "," + maxCoord.x + "," + maxCoord.y
                    layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
                    zoomEnded(envelope)
                } // else if targetRectangle is a horizontal or a vertical strip, do nothing
            }
            targetRectangle.disable()
            zoomStarted = false
            showInfo = true
            layerManager.refresh()
            var enablePanAndZoomOut = layerManager.rootLayer.scrollInfo.xsizeperc < 1.0 || layerManager.rootLayer.scrollInfo.ysizeperc < 1.0
            setZoomPanButton(enablePanAndZoomOut)
            checkZoomNormalButton(enablePanAndZoomOut)
            layerManager.zoomOutMode = enablePanAndZoomOut
        } else if ( layerManager.panningMode ){
            if (panStarted){
                var dX = mouseX - panPrevMouseX
                var dY = mouseY - panPrevMouseY
                panPrevMouseX = mouseX
                panPrevMouseY = mouseY
                var cbZoom = layerManager.rootLayer.zoomEnvelope
                var deltax = (cbZoom.minx - cbZoom.maxx) * dX / width;
                var deltay = (cbZoom.maxy - cbZoom.miny) * dY / height;
                cbZoom.minx += deltax;
                cbZoom.maxx += deltax;
                cbZoom.miny += deltay;
                cbZoom.maxy += deltay;
                var envelope = cbZoom.minx + "," + cbZoom.miny + "," + cbZoom.maxx + "," + cbZoom.maxy
                layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
                zoomEnded(envelope)
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
        if ( layerManager){
            var envelope = layerManager.rootLayer.zoomEnvelope
            Global.calcZoomOutEnvelope(envelope, layers, viewmanager, wheel.angleDelta.y < 0 ? 0.9 : -0.2 )
            zoomEnded(envelope)
        }
    }
}


