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
    property MapScrollers mapScrollers
    property bool showInfo : true
    property bool hasPermanence : false
    property bool zoomStarted : false
    property bool panningStarted : false
    property int panningPrevMouseX : -1
    property int panningPrevMouseY : -1
    property int panningDirection : Global.panningReverse
    property string selectiondrawerColor : "basic"
    property string subscription
    signal zoomEnded(string envelope)

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        if ( layerview.tabmodel){
            if (!layerview.tabmodel.selected)
                layerview.tabmodel.selectTab()
        }
        if ( layerManager.zoomInMode ){
			if (!zoomStarted){
				zoomRectangle.x = mouseX
				zoomRectangle.y = mouseY
				zoomRectangle.visible = true
				showInfo = false
			}
            zoomStarted = true
        }

        if ( layerManager.panningMode ){
            panningStarted = true
            panningPrevMouseX = mouseX
            panningPrevMouseY = mouseY
            cursorShape = Qt.ClosedHandCursor

        }
        if ( showInfo && layerManager.rootLayer.showLayerInfo && !zoomStarted){
          floatrect.enabled = true
          floatrect.opacity = 1
          floatrect.x = mouseX
          floatrect.y = mouseY
          var mposition = mouseX + "|" + mouseY
          floatrect.text = layerManager.rootLayer.layerInfo(mposition)
        }
    }

    function panMapArea(dX, dY) {
        mapScrollers.vscroller.scroll(panningDirection * dY, true)
        mapScrollers.hscroller.scroll(panningDirection * dX, true)
    }

    onPositionChanged: {
		if (zoomStarted){
			var aspect = width / height
			if ( zoomRectangle.x < mouseX)
				zoomRectangle.width = mouseX - zoomRectangle.x
			else {
				zoomRectangle.width = Math.abs(mouseX - zoomRectangle.x) 
				zoomRectangle.x = mouseX
			}
			zoomRectangle.height = zoomRectangle.width / aspect
			if ( zoomRectangle.y > mouseY){
				zoomRectangle.y = mouseY
			}
		}
/*        if (!layerManager.panningMode) {
            cursorShape = Qt.ArrowCursor
        } else {
            if (!panningStarted)
                cursorShape = Qt.OpenHandCursor
            else
                cursorShape = Qt.ClosedHandCursor
        }

        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition

        var dX = mouseX - panningPrevMouseX
        var dY = mouseY - panningPrevMouseY

        if ( (zoomStarted || panningStarted) && layerManager.hasSelectionDrawer){
            var position;

            if (!panningStarted) { // zooming.....
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
                    panningPrevMouseX = mouseX
                    panningPrevMouseY = mouseY
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
		    var minPos = {x: zoomRectangle.x-30, y: zoomRectangle.y-30, z: 0}
			var maxPos = {x: zoomRectangle.width + zoomRectangle.x-30, y: zoomRectangle.height + zoomRectangle.y-30, z: 0}
			var minCoord = manager.rootLayer.screen2Coord(minPos)
            var maxCoord = manager.rootLayer.screen2Coord(maxPos)
			var envelope = minCoord.x + "," + minCoord.y + "," + maxCoord.x + "," + maxCoord.y

            if (zoomStarted)
                layerManager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");

			zoomRectangle.disable()
            zoomStarted = false
			showInfo = true
			layerManager.refresh()
        }
        if ( layerManager.panningMode ){
            panningStarted = false
            panningPrevMouseX = -1
            panningPrevMouseY = -1
            cursorShape = Qt.OpenHandCursor
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


