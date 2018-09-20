import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

DropableItem {
    id : dropContainer
    width : 155
    maxHeight: 190
	property var layermanager

    property var panelCallBack
    Rectangle {
	    anchors.fill: parent
        color : "white"
        border.width: 1
        border.color: Global.edgecolor
        radius: 2

		Column {
			width : Math.min(400, parent.width - 10)
			height : parent.height - 5
			spacing : 4
			x : 8
			y : 4
			Button {
				id : prevZoom
				text : "Back to previous zoom"
				width : 140
				height : 25

				onClicked : {
					if ( layerview.lastZoomEnvelope != ""){
					     var command = "setviewextent("+ layermanager.viewid + "," + layerview.lastZoomEnvelope + ")"
						layermanager.addCommand(command);
                        layerview.broadCastNewExtent(layermanager, layerview.lastZoomEnvelope)
						toggle()
					}
				}
			}
			Item {
			    width : Math.min(400, parent.width - 10)
				height : 140
				Text {
					id : label
					text : "Zoom to layer"
					height : 18
					width : 100
					font.bold : true
				}
				Rectangle {
					id : covlayers
					clip : true
					anchors.top : label.bottom
					//anchors.topMargin : 2
					width : parent.width
					height : 120
					border.width : 1
					border.color : Global.edgecolor
					ScrollView {
						anchors.fill : parent
						anchors.margins : 1
						ListView {
						    id : maps
							anchors.fill : parent
							anchors.margins : 1
							model : layermanager ? layermanager.allCoverages : null
							//highlight: Rectangle{ width :( maps.currentIndex >= 0 ? parent.width : 0); height : 18; color : Global.selectedColor}

							delegate : 
									Rectangle {
										width : parent.width-2
										height : 18
										color: ListView.isCurrentItem ? Global.selectedColor : "white"
										id : co
										MouseArea {
											anchors.fill : parent
											onClicked :{
												var p = maps.indexAt(co.x, co.y)
												maps.currentIndex = p
												var lyr = layermanager.findLayer(nodeId)
												var env = lyr.vproperty("envelope")
												var crdMin = layermanager.rootLayer.coord2coord(lyr, env["minx"], env["miny"])
												var crdMax = layermanager.rootLayer.coord2coord(lyr, env["maxx"], env["maxy"])
												var newEnv = crdMin["x"] + ","+ crdMin["y"] + "," + crdMax["x"] + ","+ crdMax["y"]
												var command = "setviewextent("+ layermanager.viewid + "," + newEnv + ")"
												console.debug("ttttt", command)
												layermanager.addCommand(command);
												layerview.broadCastNewExtent(layermanager, newEnv)
												toggle()
											}
										}
										Row {
											anchors.fill : parent
											spacing : 6
											Image{
													id : imageBase
													x : 0
													y : 0
													width : 16; height : 16
													source : "../images/" + icon
													fillMode: Image.PreserveAspectFit
											}
    										Text {
												id: txt
												text: name
												elide: Text.ElideMiddle
												width : parent.width
												height :17
												anchors.verticalCenter: parent.verticalCenter
  												verticalAlignment: Text.AlignVCenter
												font.pointSize: 8
											}
									}
								}
							}
					}
				}
			}
		}
	}
}