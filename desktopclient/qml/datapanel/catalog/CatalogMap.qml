import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import ModelRegistry 1.0
import "../../controls" as Controls
import "../visualization" as MapTools
import "../../Global.js" as Global

Rectangle {
    width : parent.width
    id : catalogMapView
    color : tabmodel && tabmodel.side == "right" ? "white" : "#FFFFF7"

    onWidthChanged : {
        mapItems.requestPaint();
    }

    onHeightChanged : {
        mapItems.requestPaint();
    }

    function closeGrid() {
      grid.setSource("")
    }

    function changeSubPanel() {
        // dummy function
        return false
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
                renderer.layermanager.zoomInMode = !renderer.layermanager.zoomInMode
                grid.setSource("")
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( renderer.layermanager){
                var envelope = renderer.layerManager.rootLayer.zoomEnvelope;
                var zoomposition = {x: 0.5, y: 0.5};
                envelope = Global.calcZoomOutEnvelope(envelope, zoomposition, renderer.layerManager,0.707);
                renderer.newExtent(envelope.minx + " " + envelope.miny + " " + envelope.maxx + " " + envelope.maxy);
                grid.setSource("");
            }
        }
    }

    Action {
        id : normalClicked
        onTriggered : {
            if ( renderer.layermanager){
                renderer.layermanager.zoomInMode = false
                renderer.layermanager.zoomOutMode = false
                renderer.layermanager.panningMode = false
            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
                renderer.layermanager.zoomOutMode = false
                renderer.layermanager.panningMode = false
                renderer.addCommand("setviewextent("+ renderer.layermanager.viewid + ",\'entiremap\')");
                renderer.layermanager.refresh()
                grid.setSource("")
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
                layermanager = models.createLayerManager(catalogMapView,renderer)
                var cmd = uicontext.worldmapCommand(layermanager.viewid)
                renderer.addCommand(cmd)
                mouseActions.layerManager = layermanager
                renderer.addCommand("setlinecolor(" + layermanager.viewid + ", 1,darkblue)");
                renderer.addCommand("setbackgroundcolor(" + layermanager.viewid + "," + catalogMapView.color + ")")
                renderer.update()
            }
        }

        Connections {
            target: mouseActions
            onZoomEnded :{
                renderer.layermanager.addCommand("setviewextent("+ renderer.layermanager.viewid + "," + envelope + ")");
            }
        }

        Connections {
            target : mouseActions
            onClick :{
                var maps = []
                for (var i = 0; i < mapItems.items.length; i++) {
                    var env = mapItems.items[i].getProperty("latlonenvelope")
                    if ( env === "?")
                        continue;

                    var envelope = renderer.rootLayer().drawEnvelope(env)
                    if ( envelope.minx <=mx && (envelope.minx + envelope.width) > mx &&
                            envelope.miny <= my && (envelope.miny + envelope.height) >my ){
                        maps.push({"name" : mapItems.items[i].name,
                                      "imagePath" : mapItems.items[i].imagePath,
                                      "id" : mapItems.items[i].id,
                                      "iconPath" : mapItems.items[i].iconPath,
                                      "url" : mapItems.items[i].url,
                                      "typeName" : mapItems.items[i].typeName })
                    }
                }
                if ( maps.length > 0 && !renderer.layermanager.zoomInMode){
                    grid.setSource("") // remove old grid
                    grid.setSource("SelectedSpatialItems.qml",{"x" : mx + 20, "y" : my -20, "model" : maps})
                    grid.active  = true
                }
                if (catalogViews && catalogViews.tabmodel && !catalogViews.tabmodel.selected)
                    catalogViews.tabmodel.selectTab()
             }
        }

        Controls.LayerExtentMouseActions{
		    id : mouseActions
		    zoomToExtents: true
		    hasPermanence: false
		    showInfo: false
		    selectiondrawerColor: "basic"
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
        property var zoomEnvelope : renderer.layermanager.rootLayer.zoomEnvelope

        onItemsChanged: {
            mapItems.requestPaint()
        }
        onZoomEnvelopeChanged : {
            mapItems.requestPaint();
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

