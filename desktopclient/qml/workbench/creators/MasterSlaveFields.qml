import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import LayerManager 1.0
import ModelRegistry 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
	width : parent.width
	height : 50
	Row {
		width : parent.width
		height : 20
		Controls.TextEditLabelPair {
			id : refraster
			labelText : qsTr("Master Raster")
			labelWidth : 120
			width : parent.width - 20
			height : 20
			checkFunction : testDrop
			onDropped : {
				var ilwisobjectid = drag.source.ilwisobjectid
				var filter = "itemid=" + ilwisobjectid
				var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "right")
				tiepointstable.editor.linkModels(tab.item.layermanagers[0])
				tab.item.tabmodel.displayName = "Master Raster"
			}
		}
		Button {
			width : 20
			height : 20
			iconSource : "../../images/view.png"

			onClicked : {
				var filter = "itemid="+ refraster.ilwisobjectid
				var tab = bigthing.newCatalog(filter ,"rastercoverage",refraster.content, "right")
				tiepointstable.editor.linkModels(tab.item.layermanagers[0])
				tab.item.tabmodel.displayName = "Master Raster"
			}
		}
	}
	Row {
		width : parent.width
		height : 20
		Controls.TextEditLabelPair {
			id : backraster
			labelText : qsTr("Slave raster")
			labelWidth : 120
			width : parent.width - 20
			height : 20
			checkFunction : testDrop
			onDropped : {
				var ilwisobjectid = drag.source.ilwisobjectid
				var filter = "itemid=" + ilwisobjectid
				var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "left")
				if ( "layermanagers" in tab.item){
					tiepointstable.editor.slaveLayerManager(tab.item.layermanagers[0],ilwisobjectid)
					tab.item.setActiveEditor(tiepointstable)
					tab.item.activeLayerManager().addPostDrawer(tiepointstable.editor)
					tab.item.tabmodel.displayName = "Slave Raster"
				}

			}
		}
		Button {
			width : 20
			height : 20
			iconSource : "../../images/view.png"

			onClicked : {
				var filter = "itemid="+ backraster.ilwisobjectid
				var tab = bigthing.newCatalog(filter ,"rastercoverage",backraster.content, "left")
				tiepointstable.editor.slaveLayerManager(tab.item.manager,backraster.ilwisobjectid)
				tab.item.setActiveEditor(tiepointstable)
				tab.item.activeLayerManager().addPostDrawer(tiepointstable.editor)
				tab.item.tabmodel.displayName = "Slave Raster"
			}
		}
	}

    function testDrop(id){
        if (!id)
            return false
        var obj = mastercatalog.id2object(id, null)
        if ( obj && obj.typeName === "rastercoverage"){
			obj.suicide()
            return true
        }
		if ( obj)
			obj.suicide()
        return false
    }
}