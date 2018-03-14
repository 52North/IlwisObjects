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
        tree.hoveredRow = -1
        tree.rowBeingMoved = -1
        tree.dropHandled = true
		var layer = manager.findLayer(currentNodeId)
		if ( layer){
			var index = layer.index
			tree.expand(index)
		
			tree.selection.setCurrentIndex(index, ItemSelectionModel.ClearAndSelect)
			setTreeIndex(index)
		}
		
	}

	function setTreeIndex(index){
		var idx = manager.nodeid(index)
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
                        var lyr = manager.findLayer(currentNodeId)
                        if ( lyr ){
                            var fixed = false // you can't create composite layers in a fixed layer
                            if ( lyr.parentLayer)
                                fixed = lyr.parentLayer.hasFixedStructure
                            if ( fixed) // if fixed the drop container never shows
                                dropContainer.state = "invisible"
                            else
                                dropContainer.state = dropContainer.state == "invisible" ? "visible" : "invisible"
                                
                        }

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
                                    var parentLayer = lyr.parentLayer ? lyr.parentLayer.nodeid : ""
									var cmd = 'adddrawer(' + manager.viewid + ',\"' + parentLayer + '\",,compositelayer,true, \"' + newname.content + '\",' + currentNodeId + ')'
                                    console.debug("xxxxxx", cmd)
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
                property bool dropHandled : false
                property var hoveredRow : -1
                property var rowBeingMoved : -1
				selection: ItemSelectionModel {
					model: tree.model
				}

				rowDelegate : Column {
                    width : 250
                    height : 22
                    
                    Rectangle {
                        width : 200
                        height : 2
                        border.width : 1
                        border.color : Global.edgecolor
                        opacity : tree.hoveredRow == styleData.row ? 1 : 0 
                    }
                     Rectangle {
                        y : 2
					    width : 200
					    height : 19
					    color : styleData.selected ? Global.selectedColor : "white"
				    }
                }
				itemDelegate: LayerDelegate {
				}
				
			 	TableViewColumn {
                    title: "Name"
                    role: "data"
                    resizable: true
                    width : 150
                }

                Component.onCompleted : {
                    if ( flickableItem ) {
                        flickableItem.interactive = false
                    }
                }
            }
            
			onDropped : {
                if (!tree.dropHandled){
				    var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
				    if ( resource){
					    layerview.addDataSource(resource.url, resource.name, resource.typeName) 

					    setModel()
				    }
                } 
                tree.dropHandled = false
			}
        }

    }

	function addData(objectid){
	
	}
}
