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
            worldmap.layermanager.addCommand("setviewextent("+ worldmap.layermanager.viewid + ",entiremap)");
            worldmap.layermanager.refresh
            if ( filterTarget){
                if ( "spatialFilter" in filterTarget)
                    filterTarget.spatialFilter = ""
            }
        }else
            worldmap.layermanager.rootLayer.vproperty("viewextent", currentEnvelope)
    }



    Rectangle{
        id : worldmapcontainer
        width : parent.width
        height : 250
        property LayerManager manager

        SpatialSelectionToolbar{
            id : buttons
            anchors.right: worldmap.left
        }
        Controls.DummySpatial{
             id : worldmap
             height : 240
             x : 28
             y : 2
        }

    }

    Component.onCompleted: {
        worldmap.layermanager = uicontext.createLayerManager(selector,worldmap)
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

