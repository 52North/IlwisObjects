import QtQuick 2.0
import UIContextModel 1.0
import ".." as Model

Model.ModellerWorkArea {
    id : workarea
    property var currentAnalysis
	property var analysisManager

    onCurrentAnalysisChanged: {
        updateView1()
    }

    state : "invisible"


    onStateChanged: {
        if ( state == "visible"){
            if ( !meme.item){
                updateView1()
            }else{
               meme.item.visible = true
                meme.item.enabled = true
            }
        }else if ( state == "invisible"){
            if ( meme.item){
                meme.item.visible = false
                meme.item.enabled = false
            }
        }
    }

    Loader {
        anchors.fill: parent

        onLoaded :{
            item.visible = true
        }

        id: meme
    }

    function updateView1() {
        meme.setSource((currentAnalysis ? (uicontext.ilwisFolder  + currentAnalysis.panel("main")) : "")) 
    }

	function view() {
		if ( meme.item)
			return meme.item
		return null
	}

	 Component.onCompleted: {
	    height = parent.height
	 }
}

