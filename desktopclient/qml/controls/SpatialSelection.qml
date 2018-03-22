import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import "../controls" as Controls
import "../datapanel/visualization" as MapTools
import UIContextModel 1.0
import LayerManager 1.0

Item {
    id : selector
    property var currentEnvelope
    property var filterTarget
    clip : true


    onCurrentEnvelopeChanged: {
        if ( currentEnvelope === "entiremap"){
            if ( filterTarget){
                if ( "spatialFilter" in filterTarget)
                    filterTarget.spatialFilter = ""
            }
        }else
            worldmapcontainer.manager.rootLayer.vproperty("viewextent", currentEnvelope)
    }



    Row{
        id : worldmapcontainer
        width : parent.width
        height : 250
        property LayerManager manager

        SpatialSelectionToolbar{
            id : buttons
            //anchors.right: worldmap.left
        }
        Controls.DummySpatial{
             id : worldmap
             height : 240
             width : parent.width - buttons.width
             layermanager : worldmapcontainer.manager

             //x : 28
             //y : 2

            Connections{
                target : mouseActions
                onZoomEnded : {
                    var env = envelope.replace(/,/g , " ")
                    if ( filterTarget){
                            filterTarget.filter("spatial", env)
                    }
                    currentEnvelope = envelope
                    worldmapcontainer.manager.addCommand("setviewextent("+ worldmapcontainer.manager.viewid + "," + envelope + ")");
                }
            }
            LayerExtentMouseActions{
                id : mouseActions
                layerManager: worldmapcontainer.manager
	            zoomToExtents: true
			    hasPermanence: false
			    showInfo: false
			    selectiondrawerColor: "basic"
            }
        }

    }

    Component.onCompleted: {
         if (! worldmapcontainer.manager){
                worldmapcontainer.manager = uicontext.createLayerManager(worldmapcontainer,worldmap)
                var cmd = uicontext.worldmapCommand(worldmapcontainer.manager.viewid)
                worldmapcontainer.manager.addCommand(cmd)
                worldmapcontainer.manager.addCommand("setlinecolor(" + worldmapcontainer.manager.viewid + ", 1,darkblue)");
                worldmapcontainer.manager.addCommand("setbackgroundcolor(" + worldmapcontainer.manager.viewid + ",white)")
            }
    }
    states: [
        State { name: "invisible"

            PropertyChanges {
                target: selector
                height : 0
            }

        },
        State {
            name : "visible"
            PropertyChanges {
                target: selector
                height : 270
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation
            { properties: "height"; duration : 500 ; easing.type: Easing.OutCubic
            }

        }
    ]
}

