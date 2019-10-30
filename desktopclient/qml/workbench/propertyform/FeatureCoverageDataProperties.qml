import QtQuick 2.0
import QtQuick.Controls 1.0 as QC1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../Global.js" as Global
import "../../controls" as Controls


Rectangle {
    id : container
    anchors.fill : parent
    color : uicontext.lightestColor

    function storeData() {
        if ( propertyForm.editable){
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

    Text { id : line1; text : qsTr("Polygon features"); width: 100; font.italic: true }
    Text { text : getProperty("polygoncount");  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Line features"); width: 100; font.italic: true; anchors.top : line1.bottom }
    Text { text : getProperty("linecount"); width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}
    Text { id : line3; text : qsTr("Point features"); width: 100; font.italic: true; anchors.top : line2.bottom }
    Text { text : getProperty("pointcount"); width: parent.width - line3.width - 2; anchors.left: line3.right; anchors.top : line2.bottom}
    Text { id : line4; text : qsTr("Total features"); width: 100; font.italic: true; anchors.top : line3.bottom }
    Text { text : getProperty("featurecount"); width: parent.width - line4.width - 2; anchors.left: line4.right; anchors.top : line3.bottom}


    Text {
        id : titleText
        text : qsTr("Attributes")
        font.bold: true
        font.pointSize: 9
        height : 25
        anchors.top : line4.bottom
        anchors.topMargin: 4
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        anchors.top : titleText.bottom
       height : 300
        width:parent.width
        clip : true
        border.width: 1
        border.color : Global.edgecolor
 		QC1.TableView {
			id : itemtable
			width : parent.width
			height : 150
//			selectionMode : SelectionMode.ExtendedSelection
			model : attributes

			onCurrentRowChanged : {
					if ( itemtable.currentRow >= 0) {
						var id = itemtable.model[itemtable.currentRow].representationId
					
						if ( id != "" && itemtable.model[itemtable.currentRow].attributeInternalDomainType == "itemdomain") {
							var obj = mastercatalog.id2object(id,container)
							itemdom.rpr = obj
						}else{
							itemdom.rpr = null
						}
					}
				}


			QC1.TableViewColumn{
				title : qsTr("Attribute");
				role : "attributename" 
				width : 100
				delegate: 	Text {
					text: styleData.value
					verticalAlignment:Text.AlignVCenter
					font.pixelSize: 10
					elide: Text.ElideMiddle
					x : 4
				}
							
			}
			QC1.TableViewColumn{
				title : qsTr("Used");
				width : 100
				role : "attributeDomain"
				delegate:	Text {
					text: styleData.value
					verticalAlignment:Text.AlignVCenter
					font.pixelSize: 10
					elide: Text.ElideMiddle
					x : 4

				}
			}
			QC1.TableViewColumn{
				title : qsTr("ValueType");
				width : 120
				role : "attributeValueType"
				delegate:	Text {
					text: styleData.value
					verticalAlignment:Text.AlignVCenter
					font.pixelSize: 10
					elide: Text.ElideMiddle
					x : 4
				}
			}
			QC1.TableViewColumn{
				title : qsTr("Range");
				width : 150
				role : "range"
				delegate:	Text {
					text: styleData.value
					verticalAlignment:Text.AlignVCenter
					font.pixelSize: 10
					elide: Text.ElideMiddle
					x : 4
				}
			}
			rowDelegate: Rectangle {
				id : rowdelegate
				height :20
				color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")


				
			}

		}

		ItemDomainItems {
			anchors.top : itemtable.bottom
			anchors.topMargin : 5
			id : itemdom
			width : parent.width
			height : 200
			visible : rpr != null
		}
    }
}
