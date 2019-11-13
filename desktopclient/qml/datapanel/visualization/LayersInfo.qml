import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Column {
    id : layersinfo
    width : viewmanager.width
    height: viewmanager.height
    property string iconName : "../images/info"

    function iconSource(name) {
        if ( name === "" || name === undefined){
            return ""
		 }
         var iconP = "../../images/" + name
         return iconP

     }
    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Attribute information on Mouse Position")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
	DropArea {
		width : parent.width
		height : parent.height - header.height - 2
		onDropped: {
	var idlist = drag.source.ids.split("|")
				for(var i=0 ; i < idlist.length; ++i){
					var resource= mastercatalog.id2Resource(idlist[i])
					if ( resource.typeName == "rastercoverage" || 
						resource.typeName == "featurecoverage" ||
						resource.typeName == "pointcoverage" ||
						resource.typeName == "linecoverage" ||
						resource.typeName == "polygoncoverage"){
						activeLayerManager().addInfoLayer(resource.id)
					}
					resource.suicide()
				}

		}
		GridView{
		    id : grid
			anchors.fill : parent
			model : activeLayerManager().rootLayer.layerInfoItems //manager ? manager.rootLayer.layerInfoItems : null

			cellWidth : 240
			cellHeight : 15
			flow: GridView.FlowTopToBottom
			clip : true

			delegate: Component{
				Item{
					width : 260
					height : 15
					MouseArea { 
						acceptedButtons: Qt.AllButtons
						anchors.fill : parent
						onPressed : {
							if ( mouse.button == Qt.RightButton){
								menu.popup();
							}
							if ( mouse.button == Qt.LeftButton) {
								var temp = grid.model
								var idx = index
								if (uicontext.keyPressed(Qt.Key_Shift)) {
									var first = idx
									for(var i=0; i < grid.model.length; ++i) {
										if ( temp[i].selected){
											first = i
											break
										}
									}
									var start, end
									if ( first >= idx){
										start = idx
										end = first
									} else{
										start = first
										end = idx
									}
									for(var i=0; i < grid.model.length; ++i){
										temp[i].selected = i >= start && i <=end 
									}
									
								}
								else {
									if (!uicontext.keyPressed(Qt.Key_Control)) {
										for(var i=0; i < grid.model.length; ++i) {
											temp[i].selected = false
										}
									}
									temp[idx].selected = !temp[idx].selected
								}
								grid.model = temp
							}
						}
					}
					Rectangle {
						id : bg
						width : parent.width
						height : 15
						color : grid.model[index].selected ? Global.selectedColor : "transparent"
						Row {
							width : parent.width
							height : 15
							spacing: 3
							Image {
								source : iconSource(modelData.icon)
								width : 14
								height : 14
							}

							Text{
								text : modelData.name
								width : 100
								height : 15
								font.bold : true
								color: modelData.color
								elide: Text.ElideMiddle
							}
							Text {
								text : modelData.value
								width : 140
								height : 15
								clip : true
								color: modelData.color 
								font.bold : modelData.header ? true : false
								elide: Text.ElideMiddle

							}
						}
					}

				}
			}
		}

	}
    Menu {
          id: menu
          MenuItem {
              text: "copy"
              onTriggered: {
					var data = []
					for(var i=0; i < grid.model.length; ++i){
						if ( grid.model[i].selected) {
							var d = { 'key' : grid.model[i].name, 'value': grid.model[i].value, 'context' : grid.model[i].layer}
							data.push(d)
						}
					}
					if ( data.length > 0)
						uicontext.setClipBoardData(data, "layerinfo")
              }
          }
      }

}

