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


Item {
    id : viewmanager

   // property var renderer

   // property alias metadata : metadata

    signal zoomEnded(string envelope)

    Layout.minimumHeight: Global.actionBarMinHeight



     ViewManagerTabs{
        id : vmyab1
        visible : true
     }
     ViewManagerTabs{
        id : vmyab2
     }
     ViewManagerTabs{
       id : vmyab3
     }
     ViewManagerTabs{
        id : vmyab4
     }

    function activateViewManager(index){
        vmyab1.visible = false
        vmyab2.visible = false
        vmyab3.visible = false
        vmyab4.visible = false
        if ( index == 0)
            vmyab1.visible = true
        if ( index == 1)
            vmyab2.visible = true
        if ( index == 2)
            vmyab3.visible = true
        if ( index == 3)
            vmyab4.visible = true
    }

    function activeSubPanel(){
        if ( layerview.activeSubPanel === 0)
            return vmyab1
        if ( layerview.activeSubPanel === 1)
            return vmyab2
        if ( layerview.activeSubPanel === 2)
            return vmyab3
        if ( layerview.activeSubPanel === 3)
            return vmyab4
        return null
    }

    function addDataSource(filter, sourceName, sourceType){
        if ( filter.indexOf("=") !== -1 && filter[0] != '\"'){
            filter = "\"" + filter + "\""
        }
        var tab = activeSubPanel().getTab(2)

        if ( tab && tab.item ){
            tab.item.addDataSource(filter, sourceName, sourceType)

        }

		tab = activeSubPanel().getTab(0)
		if ( tab && tab.item){
			tab.item.setModel()
			activeLayerManager().refresh()
		}
    }

    function transfer(datapanel){
        var tab = activeSubPanel().getTab(2)
        if ( tab){
            tab.item.transfer(datapanel)
        }
    }

    function entireMap() {
    }

     function newZoomExtent(newenvelope){
         var tab = activeSubPanel().getTab(2)
         if ( tab){
             tab.item.newZoomExtent(newenvelope)
         }
     }
    
}

