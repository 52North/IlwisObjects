import QtQuick 2.2
import QtQuick.Controls 1.1 as QC1
import QtQuick.Controls 2.3 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0
	property var itemDomain
	property var colors : []

	onItemDomainChanged : {
		var n = itemDomain.domainitems.length
		for(var  i=0; i < n ; ++i)
			colors.push(null)
	}

    clip:true

	Controls.ColorPicker2 {
		property var basey : 0
		id : picker
		width :240
		height : 20
		visible : false

		Component.onCompleted : {
			basey = picker.y
		}
	}

    Rectangle {
        id : inner
        height: 520
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5

		Column {
			id : tablepart
			width : parent.width
			height : parent.height - 20
			spacing : 4
			IlwisObjectCommon{
				id : objectcommon
			}
			Controls.TextEditLabelPair {
				id : itemdompart
				width : parent.width
				height : 20
				labelWidth : 100
				labelText : qsTr("Item Domain")
				checkFunction : itemdompart.testDrop

				function testDrop(id){
					if (!id)
						return false
					var ok = false
					var obj = mastercatalog.id2object(id, null)
					if ( obj) {
						if (obj.typeName === "itemdomain"){
								itemDomain = obj
								ok = true
						}
						if (!ok)
							obj.suicide()

					}
				
					return ok
				}
			}

			QC1.TableView {
				id : itemtable
				width : parent.width
				height : 250
	//			selectionMode : SelectionMode.ExtendedSelection
				model : itemDomain ? itemDomain.domainitems : null

				QC1.TableViewColumn{
					title : qsTr("Item name");
					role : "name"
					width : 150
					delegate: 	Text {
							text: styleData.value
							verticalAlignment:Text.AlignVCenter
							font.pixelSize: 10
							elide: Text.ElideMiddle
							x : 4
							MouseArea {
									anchors.fill: parent
									onClicked: {
										picker.visible = false
									}
								}
							}
						
				}
				QC1.TableViewColumn{
					title : qsTr("Portrayal");
					width : parent.width - 160
					delegate:	Rectangle {
						id : local
						width : parent.width
						color : colors[styleData.row] == null ? ( styleData.selected ? Global.selectedColor :( (styleData.row % 2 !== 0)? "#eee" : "#fff")) : colors[styleData.row]

						function selectColor(clr){
							colors[styleData.row] = clr
							local.color = clr
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								var p = picker.mapFromItem(local, local.x, local.y) 
								picker.x = picker.x + p.x
								picker.y = picker.y + p.y + 2
								picker.width = local.width - 5
								picker.initialColor = local.color
								picker.visible = true
								picker.closeCallBack = local.selectColor
							}
						}
					}
				}
				rowDelegate: Rectangle {
					id : rowdelegate
					height :25
					color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
				
				}

			}
		}

		ApplyCreateButtons {
            width : parent.width
            height : 60
            anchors.bottom: tablepart.bottom
            anchors.bottomMargin: 8
            id : applyButton
            createObject: apply
        }


	}

    function apply(overwrite) {
	   var itemstring = ""
       if ( itemDomain){
			var items = itemDomain.domainitems
            for(var i = 0; i < items.length; ++i){
                if (itemstring !== "")
                    itemstring += "|"
                itemstring += items[i].name;
                    itemstring += "|"+ colors[i]
            }
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ objectcommon.name, "domain")){
                    return false;
                }
            }
            var url = applyButton.currentCatalogCorrectUrl() + "/"+objectcommon.itemname
            var createInfo = {type : "representation", valuetype : "item", name :  url, items : itemstring, domain : itemdompart.content }
            var ilwisid = objectcreator.createObject(createInfo)

            return true;
		}
    }
}
