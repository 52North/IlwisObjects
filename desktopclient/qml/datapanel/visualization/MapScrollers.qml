import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Item {
    id : mapscrollers

    property alias vscroller : vscroller
    property alias hscroller : hscroller
    property string subscription
	property var needUpdate : layerContainer.layerManager() ? layerContainer.layerManager().needUpdate : null
    signal scrolled(string envelope)

	onNeedUpdateChanged : {
        if ( layerContainer.layerManager()) {
		    var scrollInfo = layerContainer.layerManager().rootLayer.scrollInfo	
		    hscroller.currentSize = scrollInfo.xsizeperc * 100
		    hscroller.currentPosition = scrollInfo.leftpositionperc * 100
		    vscroller.currentSize = scrollInfo.ysizeperc * 100
		    vscroller.currentPosition = scrollInfo.toppositionperc * 100
        }
	}

    Controls.HScrollBar{
        anchors.bottom : parent.bottom
        id :hscroller
        objectName: "hscrollbar" + uicontext.uniqueName()

		anchors.leftMargin : 30
		anchors.rightMargin : 30

        onScrolled: {
			var scrollInfo = layerContainer.layerManager().rootLayer.scrollInfo	
			scrollInfo.leftpositionperc = position / 100
			layerContainer.layerManager().rootLayer.scrollInfo = scrollInfo
             if ( viewmanager){
                   broadCastNewExtent(layerContainer.layerManager(), layerContainer.layerManager().rootLayer.zoomEnvelope) 
                   viewmanager.newZoomExtent(layerContainer.layerManager().rootLayer.zoomEnvelope)
             }
        }

		onScrolledStopped : {
			layerContainer.layerManager().updateBackground()
		}
    }
    Controls.VScrollBar{
        anchors.right : parent.right
        id :vscroller
        objectName: "vscrollbar" + uicontext.uniqueName()

        onScrolled: {
			var scrollInfo = layerContainer.layerManager().rootLayer.scrollInfo
				
			scrollInfo.toppositionperc = position / 100
			layerContainer.layerManager().rootLayer.scrollInfo = scrollInfo
             if ( viewmanager){
                   broadCastNewExtent(layerContainer.layerManager(), layerContainer.layerManager().rootLayer.zoomEnvelope)
                   viewmanager.newZoomExtent(layerContainer.layerManager().rootLayer.zoomEnvelope)
             }
        }

		onScrolledStopped : {
			layerContainer.layerManager().updateBackground()
		}
    }
}

