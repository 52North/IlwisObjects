import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "./propertyeditors" as DisplayOptions
import UIContextModel 1.0
import LayerManager 1.0
import ".." as DataPanel
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base



TabView{
    id : layersmeta
    anchors.fill: parent
    tabPosition: Qt.BottomEdge
    visible : false

    Component {
        id : displayOptions
        DisplayOptions.LayerPropertyManagement{
		    width : parent.width
		    id : layersTree
	    }
    }

    Component {
        id : layersinfo
        LayersInfo{
        }
    }

    Component{
        id : metadata
        MetaData{
            id : innermetadata
        }
    }

    onCurrentIndexChanged: {
        var tab= getTab(2)
        tab.item.drawerActive = currentIndex == 2

    }

    function tabClicked(index){
        if ( currentIndex === index ||  layouts.state == "" ){
            if ( viewmanager.height <= 60){
                layouts.state = "visible"
            }
            else{
                layouts.state = ""
                layouts.state = "invisible"
            }
        }else {
			if ( currentIndex !== index ){
				if ( layouts.state = "invisible")
					layouts.state = "visible"
			}
		}

        currentIndex = index
    }

    function endZoom(envelope) {
        zoomEnded(envelope)
    }

    Component.onCompleted: {
        var tab =addTab(qsTr("Actions for Display"), displayOptions)
        tab.active = true

        tab = addTab(qsTr("Layers Info"), layersinfo)
        tab.active = true
        tab = addTab(qsTr("Metadata"), metadata)            
        tab.active = true // we need to be active as layers may be added to it
    }

    style: DataPanel.ButtonBarTabViewStyle{}
}