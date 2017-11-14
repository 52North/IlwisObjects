import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0

MouseArea {
    id : mapArea
    anchors.fill: parent
    hoverEnabled: true
    property LayerManager layerManager
    property bool zoomStarted : false
    signal zoomEnded(string envelope)
    signal click(int x, int y)
    signal press(int x, int y)

    FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        if ( layerManager.zoomInMode ){
            if ( !zoomStarted){
                layerManager.addCommand("removedrawer(" + drawer.viewid + ",selectiondrawer,post)");
                layerManager.hasSelectionDrawer = false
            }

            if ( !layerManager.hasSelectionDrawer){
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true
                layerManager.addCommand("adddrawer(" + drawer.viewid + ",selectiondrawer)")
                layerManager.setAttribute("selectiondrawer", position)
                layerManager.refresh()
            }
            zoomStarted = true
            press(mouseX, mouseY)

        }

    }
    onPositionChanged: {
        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition
        if ( zoomStarted && layerManager.hasSelectionDrawer){
            var position = {currentx: mouseX, currenty:mouseY}
            layerManager.setAttribute("selectiondrawer", position)
            layerManager.copyAttribute("selectiondrawer","envelope");
            layerManager.refresh()
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode && layerManager.hasSelectionDrawer){
            var envelope = layerManager.rootLayer.vproperty("selectiondrawer","envelope");
            layerManager.addCommand("setviewextent("+ layerManager.viewid + "," + envelope + ")");
            if ( envelope !== "" ){
                layerManager.hasSelectionDrawer = false
                layerManager.addCommand("removedrawer(" + layerManager.viewid + ",selectiondrawer,post)");
            }
            zoomEnded(envelope)
            zoomStarted = false
            layerManager.refresh()
        }else
            click(mouseX, mouseY)
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
}




