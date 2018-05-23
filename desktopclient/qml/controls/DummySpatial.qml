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
   Loader {
        anchors.fill: parent
        id: postDrawers
    }
    Canvas {
        id : canvas
        anchors.fill : parent
        property var needUpdate : layermanager ? layermanager.updatePostDrawers : false

        onNeedUpdateChanged : {
            canvas.requestPaint()
        }

        onPaint:{
            var ctx = canvas.getContext('2d');
            ctx.reset();
            ctx.beginPath()
            ctx.clearRect(0,0,width, height);
            var postDrawersList = layermanager.postDrawers;
            for(var i=0; i < postDrawersList.length; ++i){
                postDrawers.source = postDrawersList[i].associatedUrl
                postDrawers.item.draw(ctx, postDrawersList[i])
            }
        }
     }

    function updateView() {
		if ( layermanager){
            layersView.source = "qrc:/qml/SpatialView.qml"
			state = "visible"
		}
    }



}
