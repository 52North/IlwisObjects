import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../visualization" as MapTools
import "../../Global.js" as Global

Rectangle {
    width : parent.width
    id : catalogMapView
    color : tabmodel && tabmodel.side == "right" ? "white" : "#FFFFF7"

    function closeGrid() {
      grid.setSource("")
    }

    Action {
        id : panningClicked
        onTriggered : {
            if ( renderer.layermanager){
                renderer.rootLayer().panningMode = !renderer.rootLayer().panningMode
                grid.setSource("")
            }
        }
    }

    Action {
        id : zoomClicked
        onTriggered : {
            if ( renderer.layermanager){
                renderer.rootLayer().zoomInMode = !renderer.rootLayer().zoomInMode
                grid.setSource("")
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( renderer.layermanager){
                var envelope = renderer.rootLayer().vproperty("rootdrawer","zoomenvelope");
                Global.calcZoomOutEnvelope(envelope, renderer, renderer.layerManager,0.707)
            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
          var envelope = renderer.rootLayer().vproperty("rootdrawer","coverageenvelope");
            if ( envelope !== ""){
                renderer.addCommand("setviewextent("+ renderer.layermanager.viewid + "," + envelope + ")");
            }
            renderer.layermanager.refresh()
        }
    }

    Action {
        id : refreshClicked
        onTriggered: {
           renderer.rootLayer().viewEnvelope
            currentCatalog.prepareMapItems(renderer.layermanager, true)
            mapItems.items = currentCatalog.mapItems
            mapItems.canvasDirty = true
            mapItems.requestPaint()
        }
    }

    Controls.LayerExtentsToolbar{
        id : maptools
    }
    Controls.DummySpatial{
        id : renderer
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5

        Component.onCompleted: {
            if (! layermanager){
                layermanager = uicontext.createLayerManager(catalogMapView,renderer)
            }
        }
    }
    Canvas{
        id : mapItems
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5
        property bool canvasDirty: false
        property var items : currentCatalog ? currentCatalog.coverages : []
        property var ctx

        onItemsChanged: {
            mapItems.requestPaint()
        }

        function clear() {
            ctx.reset();
            ctx.clearRect(0, 0, width, height);
            ctx.stroke();
            mapItems.requestPaint();
        }
        Loader {
            id : grid
        }

        onPaint: {
            if ( catalogMapView.height != 0){
                if (!mapItems.ctx && mapItems.available){
                    mapItems.ctx = mapItems.getContext('2d')
                }
                if (ctx && renderer.layermanager && items ) {
                    clear(ctx);
                    canvasDirty = false
                    var l = items.length



                    for (var i = 0; i < l; i++) {
                        ctx.save()
                        var env = items[i].getProperty("latlonenvelope")
                        if ( env === "?")
                            continue;

                        var envelope = renderer.rootLayer().drawEnvelope(env)
                        if ( envelope.width > 5 && envelope.height > 5){
                            ctx.beginPath();
                            ctx.lineWidth = 1;
                            ctx.strokeStyle = "red"
                            ctx.strokeRect(envelope.minx, envelope.miny, envelope.width, envelope.height)
                            ctx.text(items[i].name,envelope.minx + 5, envelope.miny + 10)
                            ctx.stroke()
                        }else {
                            ctx.beginPath();
                            ctx.lineWidth = 1;
                            ctx.strokeStyle = "red"
                            var xc = envelope.minx + envelope.width / 2
                            var yc = envelope.miny + envelope.height / 2
                            ctx.moveTo(xc - 5, yc)
                            ctx.lineTo(xc + 5, yc)
                            ctx.moveTo(xc, yc - 5)
                            ctx.lineTo(xc, yc + 5)
                            ctx.moveTo(xc,yc)
                            ctx.arc(xc,yc,5,0, Math.PI * 2,true)
                            ctx.stroke()

                        }

                        ctx.restore()

                    }

                }
            }
        }
    }

}

