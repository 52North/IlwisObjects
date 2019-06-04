import LayerManager 1.0
import UIContextModel 1.0
import QtQuick 2.0

Rectangle {
    id : dd2
    property LayerManager layermanager
    property var viewid : layermanager ? layermanager.viewid : 0
	property var postRenderCallBack
	objectName : uicontext.uniqueName()

	onLayermanagerChanged : {
		layermanager.qmlDrawer(objectName)
		updateView()
	}

	function reset() {
		if ( layersView.item){
			layersView.item.reset()
		}
	}

	function initSizes() {
		if ( layersView.item){
			layersView.item.initSizes()
		}
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
			item.postRenderCallBack = dd2.postRenderCallBack
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
        property var needUpdate : layermanager ? layermanager.updateNeededPostDrawers : false

        onNeedUpdateChanged : {
            canvas.requestPaint()
        }

        onPaint:{
            if (layermanager) {
                var ctx = canvas.getContext('2d');
                ctx.reset();
                ctx.beginPath()
               ctx.clearRect(0,0,width, height);
                var postDrawersList = layermanager.postDrawers;
                for(var i=0; i < postDrawersList.length; ++i){
                    if ( postDrawersList[i].postDrawerActive){
                        postDrawers.source = postDrawersList[i].associatedUrl
                        postDrawers.item.draw(ctx, postDrawersList[i])
                    }
                }
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
