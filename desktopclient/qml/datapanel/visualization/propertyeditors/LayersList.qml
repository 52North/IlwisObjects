import QtQuick 2.2
import QtQuick.Controls 1.5 
import QtQuick.Controls 2.0 as QC20
import QtQml.Models 2.2
import UIContextModel 1.0
import LayerManager 1.0
import LayerModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global


Item {
    id : firstColumn
    width : 180

	property var dummy101 : manager.layerTree

	onDummy101Changed : {
		if ( manager.lastAddedCoverageLayer)
			currentNodeId = manager.lastAddedCoverageLayer.nodeId
	}

	function setModel(){
		tree.model = null
		tree.model = manager.layerTree
		var layer = manager.findLayer(currentNodeId)
		if ( layer){
			var index = layer.index(manager.layerTree)
			tree.expand(index)
		
			tree.selection.setCurrentIndex(index, ItemSelectionModel.ClearAndSelect)
			setTreeIndex(index)
		}
		
	}

	function setTreeIndex(index){
		var idx = manager.layerTree.nodeid(index)
		console.debug(index, idx)
		if ( idx >= 0){
			currentNodeId = idx
			attributeListColumn.currentCoverage = manager.findLayer(idx)
		}
	}
	property int currentNodeId : 0


    Column {
        id : layersContainer
        width : parent.width
        height : parent.height
		spacing : 2
        Rectangle {
            id : layersLabel
            width : parent.width + 10
            height : 18
            color : uicontext.paleColor
            Text{
                text : qsTr("Layers")
                font.bold: true
                x : 5
                anchors.verticalCenter: parent.verticalCenter
            }
            Controls.ToolTip{
                target: layersLabel
                text : qsTr("An ordered list of the layers visible in the map panel")
            }
        }
		Rectangle {
			width : parent.width
			height :  16
			color : uicontext.lightColor
			Row {
				id : buttonbar
				width : parent.width
				height : parent.height
				spacing : 2

				QC20.Button {
					text : qsTr("New Composite layer")	
					flat : true
					width : parent.width / 2 - 4
					height : buttonbar.height - 2
					onClicked : {
						dropContainer.state = dropContainer.state == "invisible" ? "visible" : "invisible"

					}
				}
				QC20.Button {
					text : qsTr("Remove Layer")	
					flat : true
					width : parent.width / 2 - 4
					height : buttonbar.height - 2
				}
			}
		}
		Controls.DropableItem {
			id : dropContainer
			width : parent.width
			maxHeight: 32
			 state : "invisible"
			clip:true
			z : 1000
			property var ilwisobject

			Rectangle {
				anchors.fill: parent
				color : "white"
				border.width: 1
				border.color: Global.edgecolor
				radius: 2

				Column {
					width : parent.width
					height : childrenRect.height
					x : 5
					y : 5
					spacing : 4
					Controls.TextEditLabelPair{
						id :newname
						labelWidth: 0
						height : 24
						width : parent.width - applybutton.width - 14
						y : 7
						Button {
							id :applybutton
							anchors.top : newname.top
							anchors.left: newname.right
							anchors.leftMargin: 2
							text : qsTr("Add")
							width : 60
							height : newname.height
							onClicked: {
								dropContainer.state = "invisible"
								var lyr = manager.findLayer(currentNodeId)
								if ( lyr) {
									var cmd = 'adddrawer(' + manager.viewid + ',\"' + lyr.nodeid + '\",,compositelayer,true, \"' + newname.content + '\")'
									layerview.manager.addCommand(cmd)
									manager.refresh()
									setModel()
								}
							}
						}
					}

				}
			}
		}

        DropArea {
			id : dropArea
            width : parent.width - 8
            height : parent.height - layersLabel.height - buttonbar.height - dropContainer.height - 6
            
            TreeView {
                id: tree
                width : parent.width 
                anchors.margins: 4
                focus: true
                clip : true
                height :  200
				selection: ItemSelectionModel {
					model: tree.model
				}

				onClicked : {
					tree.selection.setCurrentIndex(index, ItemSelectionModel.ClearAndSelect)
					setTreeIndex(index)
				}

				rowDelegate : Rectangle {
					width : 200
					height : 20
					color : styleData.selected ? Global.selectedColor : "white"
				}
				itemDelegate: LayerDelegate {
				}
				
				TableViewColumn {
                    title: "Name"
                    role: "data"
                    resizable: true
              
                }
            }
            
			onDropped : {
				var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
				console.debug("dropped", drag.source.ilwisobjectid, resource.name)
				if ( resource){
					layerview.addDataSource(resource.url, resource.name, resource.typeName) 

					setModel()
				} 
			}
        }

    }

	function addData(objectid){
	
	}
}
