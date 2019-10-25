import QtQuick 2.2
import QtQuick.Controls 1.1 as QC1
import QtQuick.Controls 2.3 as QC2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0
import DomainItemModel 1.0

import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    function storeData() {
        if ( propertyForm.editable){
			var colors = {} 
			for(var i=0; i < itemtable.model.length; ++i) {
				colors[itemtable.model[i].label] = itemtable.model[i].ecolor
			}
			setProperty("representationitem", colors)
			store()
        }
    }

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

    anchors.fill : parent
    Item {
        id : rprItemsId
        width : parent.width
        height : parent.height - 40
 			QC1.TableView {
				id : itemtable
				width : parent.width
				height : 250
	//			selectionMode : SelectionMode.ExtendedSelection
				model : rpritems


				QC1.TableViewColumn{
					title : qsTr("Item name");
					role : "label"
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
					title : qsTr("Presentation");
					width : parent.width - 160
					role : "ecolor"
					delegate:	Rectangle {
						id : local
						width : parent.width
						color : styleData.value

						function selectColor(clr){
							model.modelData.ecolor = clr
							//local.color = clr
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
									if (propertyForm.editable){
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
				}
				rowDelegate: Rectangle {
					id : rowdelegate
					height :25
					color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
				
				}

			}
		}

}
