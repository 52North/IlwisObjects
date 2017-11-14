import LayerManager 1.0
import UIContextModel 1.0
import QtQuick 2.0

Rectangle {
    id : dd2
    property LayerManager layermanager
    property var viewid : layermanager ? layermanager.viewid : 0


	onLayermanagerChanged : {
		updateView()
	}

    color : "grey"

    function rootLayer() {
        return layermanager.rootLayer
    }

    function addCommand(expr){
        layermanager.addCommand(expr)
    }
   Loader {
        anchors.fill: parent
        onLoaded :{
            item.visible = true
        }

        id: layersView
    }

    function updateView() {
		if ( layermanager){
            //layersView.source = uicontext.ilwisFolder + "/extensions/ui/LayersView/qml/SpatialView.qml"
            layersView.source = "qrc:/qml/SpatialView.qml"
			state = "visible"
		}
    }

}
