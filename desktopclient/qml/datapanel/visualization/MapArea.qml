import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import VisualAttribute 1.0
import QtQuick.Dialogs 1.1
import "../../controls" as Controls
import "../../Global.js" as Global

DropArea {
    id : dropArea
    property alias mouseActions : mouseActions
    property var localmanager

    function mapAreaSize() {
        return {"width" : renderer.width , "height" : renderer.height}
    }

	function drawer() {
		return renderer
	}

    onDropped: {
        var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
		if ( resource.typeName == "representation"){
			changeRepresentation(resource)
		}else {
			layerview.activeSubPanel  = layerContainer.panelSubIndex
			var idslist = drag.source.ids.split("|")
			var done = false
			if ( idslist.length == 3){
				if ( operations.testIfSuitableforCC(drag.source.ids)){
					ccquestion.ids = drag.source.ids
					ccquestion.visible = true
					done = true
				}
			} 
			if(!done){
				var idlist = drag.source.ids.split("|")
				for(var i=0 ; i < idlist.length; ++i){
					var resource= mastercatalog.id2Resource(idlist[i])
					if ( resource.typeName == "rastercoverage" || 
						resource.typeName == "featurecoverage" ||
						resource.typeName == "pointcoverage" ||
						resource.typeName == "linecoverage" ||
						resource.typeName == "polygoncoverage"){

						var filter = "itemid=" + resource.id
						addDataSource(filter, resource.name, resource.typeName) 
					}
					resource.suicide()
				}
			}
		}
    }

	MessageDialog {
		property var ids
		id : ccquestion
		title: "Use as Color Composite?"
		icon: StandardIcon.Question
		text: "Use dragged bands as Color Composite"
		standardButtons: StandardButton.Yes |StandardButton.No
		Component.onCompleted: visible = false
		onYes: {
			addDataSource("", "\"\"", 'rastercoverage','createtype=colorcomposite,items=' + ids)
		}
		onNo: {
			var idlist = ids.split("|")
			for(var i=0 ; i < idlist.length; ++i){
				var resource= mastercatalog.id2Resource(idlist[i])
				var filter = "itemid=" + resource.id
				addDataSource(filter, resource.name, resource.typeName)   
			}
		}
	}

    Rectangle {
        id : backArea
        anchors.fill: parent
        color : (layerContainer.panelSubIndex == layerview.activeSubPanel && layerview.layermanagers.length > 1 )? "#edf7f7" : "white"
		border.width : 2
		border.color :  (layerContainer.panelSubIndex == layerview.activeSubPanel && layerview.layermanagers.length > 1 )? Global.selectedColor : "white"

		Rectangle {
			id : chooseList
			anchors.centerIn : parent
			visible : false
			enabled : visible
			width: 150
			height : 150
			color : "white"
			border.width : 1
			border.color : Global.edgecolor
			radius : 4
			property var dataList
			property var formtitle 
			property var choices
			property var resource
			property var func
			z : 100
			Column {
				width : 145
				height : 143
				x : 3
				y : 5
				spacing : 2
				Label {
					width : parent.width - 6
					height : 25
					text : "  " + chooseList.formtitle
					font.bold : true
					verticalAlignment : Text.AlignVCenter
					background : Rectangle {
						color : Global.palegreen
						radius : 3
					}
				}
				ListView {
				    id : choiceList
					height : parent.height - 30
					width : parent.width - 6
					model : chooseList.choices
					highlight : Rectangle{width : parent.width;height : 18;color : Global.selectedColor}
					delegate : Text {
						x : 2
						width : parent.width
						height : 20
						text : modelData
						verticalAlignment : Text.AlignVCenter
						MouseArea {
							anchors.fill : parent
							onClicked : {
								 choiceList.currentIndex = index
								 chooseList.state = "invisible"
								 chooseList.func(chooseList.dataList[index], chooseList.resource)
							}
						}
					}
				}
			}
			states: [
            State { name: "visible"
                    PropertyChanges { target: chooseList; opacity : 1 }
                    PropertyChanges { target: chooseList; visible : true }
            },
            State {
                name : "invisble"
                    PropertyChanges { target: chooseList; opacity : 0 }
                    PropertyChanges { target: chooseList; visible : false }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "opacity"; duration : 5000 ; easing.type: Easing.InOutCubic }
            }
        ]
		}

        Rectangle {
            id : mapBorder
            anchors.fill: parent
            anchors.margins: 20

            Controls.DummySpatial{
                id: renderer
                anchors.top: parent.top
                height : parent.height
                width : parent.width
                anchors.margins: 1
                objectName : "layers_" + uicontext.uniqueName()

                Connections {
                    target: mouseActions
                    onSetZoomPanButton :{
                        layerview.activeLayerExtentsToolbar().panButton.enabled = enablePanAndZoomOut
                        layerview.activeLayerExtentsToolbar().zoomoutButton.enabled = enablePanAndZoomOut
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckZoomNormalButton :{
                        layerview.activeLayerExtentsToolbar().zoomoutButton.checked = enablePanAndZoomOut
                        layerview.activeLayerExtentsToolbar().normalButton.checked = !enablePanAndZoomOut
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckNormalButton :{
                        layerview.activeLayerExtentsToolbar().normalButton.checked = checked
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckZoomOutButton :{
                        layerview.activeLayerExtentsToolbar().zoomoutButton.checked = checked
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckPanButton :{
                        layerview.activeLayerExtentsToolbar().panButton.checked = checked
                    }
                }

                Connections {
                    target: mouseActions
                    onSelectTab :{
                        if ( layerview.tabmodel){
                            if (!layerview.tabmodel.selected)
                                   layerview.tabmodel.selectTab("7")
                        }
                    }
                }
                Connections {
                    target: mouseActions
                    onClick :{
                        layerview.activeSubPanel  = layerContainer.panelSubIndex
                        layerview.mapClicked(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onMouseMoved :{
                        layerview.mouseMoved(mx,my)   
                    }
                }

                Connections {
                    target: mouseActions
                    onMouseReleased :{
                        layerview.mouseReleased(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onMousePressed :{
                        layerview.mousePressed(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onZoomEnded :{
					    layerview.oldZoomEnvelope = layerview.lastZoomEnvelope
					    layerview.lastZoomEnvelope = envelope
						var command = "setviewextent("+ layerManager().viewid + "," + envelope + ")"
                        layerManager().addCommand(command);
                        broadCastNewExtent(layerManager(), envelope)

                        if ( typeof viewmanager != 'undefined' && viewmanager){
                            viewmanager.newZoomExtent(envelope)
                        }
                    }
                }

				Connections {
                    target: mouseActions
                    onPanChanged :{
					   layerview.oldZoomEnvelope = layerview.lastZoomEnvelope
					    layerview.lastZoomEnvelope = envelope
						var command
						if(updatebackground)
							command = "setviewextent("+ layerManager().viewid + "," + envelope + ")"
						else
							command = "setviewextent("+ layerManager().viewid + "," + envelope + ",false)"
						console.debug(command)
                        layerManager().addCommand(command);
                        broadCastNewExtent(layerManager(), envelope)

                        if ( typeof viewmanager != 'undefined' && viewmanager){
                            viewmanager.newZoomExtent(envelope)
                        }
                    }
                }
		        Controls.LayerExtentMouseActions{
			        id : mouseActions
			        layerManager: layerContainer.layerManager()
			        //maptools : layerview.activeLayerExtentsToolbar()
			        zoomToExtents: true
			        hasPermanence: false
			        showInfo: true
			        selectiondrawerColor: "basic"
		        } 
            }
        }

        Row{
            anchors.top: parent.top
            anchors.topMargin : 5
            id : toprow
            x : 20
            height : 20
            width : parent.width - 40
            Repeater {
                id : toprowRep
                model : localmanager ? localmanager.xGridAxisTop : null
                width : parent.width
                height : parent.height
                Item {
                    height : 15
                    width : modelData.size
                   // color : backArea.color
                    Text {
                        height : 15
                        width : modelData.size
                        text : modelData.value
                        x : -contentWidth / 2
                    }
                }
            }
        }
 

        Row{
            anchors.bottom: parent.bottom 
            id : bottomrow
            x : 20
            height : 20
            width : parent.width - 40
            Repeater {
                id : bottomrowRep
                model : localmanager ? localmanager.xGridAxisBottom : null
                width : parent.width
                height : parent.height
                Item {
                    y : 3
                    height : 15
                    width : modelData.size
                   Text {
                        height : 15
                        width : modelData.size
                        text : modelData.value
                        x : -contentWidth / 2
                    }
                }

            }
        }
        Column {
            anchors.left : parent.left 
            id : leftcolumn
            y : 20
            height : parent.height - 40
            width :20
            Repeater {
                id : leftcolumnRep
                model : localmanager ? localmanager.yGridAxisLeft : null
                width : parent.width
                height : parent.height
                Item {
                    height : modelData.size
                    width : 15
                    x : 3
                    Text {
                        anchors.left : parent.left
                        text : modelData.value
                        width : 15
                        height : modelData.size
                        transformOrigin: Item.TopLeft
                        rotation : -90
                        y : contentWidth / 2
                    }
                }
            }
        }

        Column {
            anchors.right : parent.right 
            id : rightcolumn
            y : 20
            height : parent.height - 40
            width :20
            Repeater {
                id : rightcolumnRep
                model : localmanager ? localmanager.yGridAxisRight : null
                width : parent.width
                height : parent.height
                Item {
                    height : modelData.size
                    anchors.right : parent.right
                    width : 20
                    Text {
                        anchors.left : parent.left
                        anchors.leftMargin : 15
                        text : modelData.value
                        width : 15
                        height : modelData.size
                        transformOrigin: Item.TopLeft
                        rotation : 90
                        y : -contentWidth / 2
                    }
                }
            }
        }
    }

    function changeSubPanel() {
        if ( layerview.activeSubPanel  != layerContainer.panelSubIndex){
            layerview.activeSubPanel  = layerContainer.panelSubIndex
            return true
        }
        return false
    }

    function assignLayerManager(newmanager){
        localmanager = newmanager
    }


    function initGeoDrawer() {
        if (!renderer.layermanager )
            renderer.layermanager = localmanager
    }

    function entireMap() {
        layerManager().addCommand("setviewextent("+ renderer.viewid + ",entiremap)");
        layerManager().refresh()
    }

    function transfer(datapanel){
        var layers = localmanager.layers;
        for(var i = layers.length-1; i > 0; --i){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
            var expr = "adddrawer(" + localmanager.viewid + ","+ layers[i].name + ",\"itemid=" + layers[i].id + "\"," + layers[i].typeName + ",true,\"\")"
            localmanager.addCommand(expr)
        }
        localmanager.refresh()
    }

	function changeRpr(layer, resource){
		layer.vproperty("visualattribute|" + layer.activeAttribute().attributename + "|representation", resource.url)
	}
	function changeRepresentation(resource){
		var coverages = localmanager.allCoverages
		var possibleLayers = []
		for(var  i=0; i < coverages.length; ++i){
			var coverage = coverages[i]
			if ( coverage.canUse(resource.id)){
				possibleLayers.push(coverage)
			}

		}
		if (possibleLayers.length === 1) {
			changeRpr(possibleLayers[0], resource)
		}else {
		    var names = []
			for(var j=0; j < coverages.length; ++j)
				names.push(coverages[j].name)

			chooseList.state = "visible"
			chooseList.choices = names
			chooseList.dataList = coverages
			chooseList.resource = resource
			chooseList.func = changeRpr
			chooseList.formtitle = "Select layer"
		}
	}

}

