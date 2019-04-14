import QtQuick 2.1
import QtQuick.Controls 1.0
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
			store()
        }
    }

    anchors.fill : parent
    Item {
        id : parentDomainItem
        width : parent.width
        height : Global.rowHeight
        Text{
            id : label1
            width : 100
            height : Global.rowHeight
            text : qsTr("Parent Domain")
        }
        Text{
            anchors.left:label1.right
            width : parent.width - label1.width
            height :Global.rowHeight
            text : getProperty("parentdomain")
        }
    }
    Item {
        id : domainItems
        anchors.top: parentDomainItem.bottom
        width : parent.width
        height : parent.height - 40
        TableView {
			id : itemtable
            anchors.fill: parent
            model : domainitems
            TableViewColumn{
                title : qsTr("Item name");
                role : "name"
                width : 100
                delegate: Component{
					Controls.TableTextField {
						readOnly : !propertyForm.editable
						onAccepted : {
							if(!readOnly){
								domainitems[styleData.row].name = text
							} 
						}
						onActiveFocusChanged : {
							if ( activeFocus){
								itemtable.currentRow = styleData.row
							}else {
								if ( text != styleData.value)
									domainitems[styleData.row].name = text
							}
						}
					}
                }
            }
            TableViewColumn{
                title : qsTr("Code");
                role : "code"
                width : 40
               delegate: Component{
					Controls.TableTextField {
						readOnly : !propertyForm.editable
						sel  : styleData.row == itemtable.currentRow
						onAccepted : {
							if(!readOnly){
								domainitems[styleData.row].code = text
							}
						}
						onActiveFocusChanged : {
							if ( activeFocus){
								itemtable.currentRow = styleData.row
							}else {
								if ( text != styleData.value)
									domainitems[styleData.row].code = text
							}
						}
					}
                }
            }
             TableViewColumn{
                title : qsTr("Description");
                role : "description"
                width : parent ? parent.width - 140 : 0
					delegate: Component{
					Controls.TableTextField {
						readOnly : !propertyForm.editable
						sel  : styleData.row == itemtable.currentRow
						onAccepted :  {
							if(!readOnly){
								domainitems[styleData.row].description = text
							}
						}
						onActiveFocusChanged : {
							if ( activeFocus){
								itemtable.currentRow = styleData.row
							}else {
								if ( text != styleData.value)
									domainitems[styleData.row].description = text
							}
						}
					}
				}
            }
			rowDelegate: Rectangle {
				id : rowdelegate
				height : 20
				color : styleData.selected ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
			}

        }
		Row{
			anchors.top : domainItems.bottom
			spacing : 5
			visible : propertyForm.editable
			Button {
				width :120
				height : 20
				text : qsTr("add new item")

				onClicked : {
					newItemDomainItem()
				}
			}
			Button {
				width :120
				height : 20
				text : qsTr("Remove selected item")
				onClicked : {
					if ( itemtable.currentRow >= 0)
						removeDomainItem(domainitems[itemtable.currentRow].name)	
				}
			}
		}
    }


}
