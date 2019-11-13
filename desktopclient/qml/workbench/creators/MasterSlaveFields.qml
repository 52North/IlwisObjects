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
	id : container
	width : parent.width
	height : 50

	property var slaveRaster
	property var masterRaster
	property var slaveLayerInfo

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
				ilwisobjectid = drag.source.ilwisobjectid
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
				if ( refraster.ilwisobjectid) {
					var filter = "itemid="+ refraster.ilwisobjectid
					var tab = bigthing.newCatalog(filter ,"rastercoverage",refraster.content, "right")
					tiepointstable.editor.linkModels(tab.item.layermanagers[0])
					tab.item.tabmodel.displayName = "Master Raster"
				}
			}
		}
	}
	Row {
		width : parent.width
		height : 22
		Controls.IlwisComboBox {
			id : slaveRasterField
			height : 20
			y : 0
			labelText: "Slave Raster"
			labelWidth : 120
			state : "collapsed"
			width : parent.width - 20
			panelHeight: slaveLayerInfo && slaveLayerInfo.length >= 3 ? 100 : 0
			checkFunction : testDrop
			callbackFunction : setSlaveRaster
			property var bandIndexes : [0,0,0]
			Rectangle {
				width : parent.width
				height : slaveRasterField.panelHeight
				parent : slaveRasterField.expandableArea
				color : Global.lightgreen
				visible : slaveRasterField.panelHeight > 0
				Column {
					x : 2
					y : 4
					z : 10
					width : parent.width - 2
					height : slaveRasterField.panelHeight - 2
					spacing : 2
					Text {
						text : qsTr("Define Color composite")
						font.bold : true
						height : 18

					}
					Controls.ComboxLabelPair{
						id : redbands
						labelText: qsTr("Red Band")
						labelWidth: 100
						width : parent.width - 5
						role : "name"
						fontSize: 8

						Connections {
							target: redbands
							onIndexChanged :{
								slaveRasterField.bandIndexes[0] = redbands.currentIndex
							}
						}
					}
					Controls.ComboxLabelPair{
						id : greenbands
						labelText: qsTr("Green Band")
						labelWidth: 100
						width : parent.width - 5
						role : "name"
						fontSize: 8

						Connections {
							target: greenbands
							onIndexChanged :{
								slaveRasterField.bandIndexes[1] = greenbands.currentIndex
							}
						}
					}
					Controls.ComboxLabelPair{
						id : bluebands
						labelText: qsTr("Blue Band")
						labelWidth: 100
						width : parent.width - 5
						role : "name"
						fontSize: 8

						Connections {
							target:bluebands
							onIndexChanged :{
								slaveRasterField.bandIndexes[2] = bluebands.currentIndex
							}
						}
					}
				}

			}

			onHeightChanged: {
				container.height = 22 + slaveRasterField.height
			}

			clip : true

		 }
		 Button {
			width : 20
			height : 20
			iconSource : "../../images/view.png"

			onClicked : {
				if ( slaveRaster && slaveRaster.id !== "") {
					var ids = slaveLayerInfo[slaveRasterField.bandIndexes[0]].id + "|" + slaveLayerInfo[slaveRasterField.bandIndexes[1]].id + "|" + slaveLayerInfo[slaveRasterField.bandIndexes[2]].id
					var options = "forcegeorefundetermined=true,createtype=colorcomposite,items=" + ids + "";
					var filter = "\"\"" 
					var tab = bigthing.newCatalog(filter ,"rastercoverage",slaveRasterField.content, "left", options)
					tiepointstable.editor.slaveLayerManager(tab.item.layermanagers[0],tiepointstable.editor.slaveid())
					tab.item.setActiveEditor(tiepointstable)
					tab.item.activeLayerManager().addPostDrawer(tab.item.layermanagers[0], tiepointstable.editor)
					tab.item.activeLayerManager().updatePostDrawers();
					tab.item.tabmodel.displayName = "Slave Raster"
				}
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

	function setSlaveRaster(url){
		slaveRaster = mastercatalog.url2object(url, "rastercoverage", container)
		if ( slaveRaster) {
			slaveLayerInfo = slaveRaster.layerInfo
			redbands.itemModel = slaveLayerInfo
			redbands.role = "name";
			greenbands.itemModel = slaveLayerInfo
			greenbands.role = "name";
			bluebands.itemModel = slaveLayerInfo
			bluebands.role = "name";
		}
	}
}