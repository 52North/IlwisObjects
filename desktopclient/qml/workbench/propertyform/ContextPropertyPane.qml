import QtQuick 2.2
import QtQuick.Controls 1.5 
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
	   id : container
    width : parent.width - 2
    height : parent.height -2
    anchors.margins: 2

    function storeData() { 
        if ( propertyForm.editable){
			for(var i=0; i < meta.model.length; ++i){
				var item = meta.model[i]
				 setAttribute("property", storeAdjustment("metadata."+ item.tag,item.value),item.tag)
			}
        }
    }
    color : uicontext.lightestColor

	Column {
	    width : parent.width
		height : 340
		Controls.TextEditLabelPair {
				id : filterField
				labelWidth : 60
				labelText : qsTr("Filter")
				width : 200
				height : 24
				transparentBackgrond : false

				onContentChanged : {
					meta.model = metaItemTable(filterField.content)	
				}
		}
		TableView {
			id : meta
			width : parent.width
			height : 300
			model : metaItemTree

			TableViewColumn{
                id : tagcolumn
                title : qsTr("Tag");
                role : "tag"
                width : 175
                delegate: 
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                        x : 4
                    }
            }
			TableViewColumn{
                id : valuecolumn
                title : qsTr("Value");
                role : "value"
                width : parent.width - tagcolumn.width
                delegate: 
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                        x : 4
                    }
            }

		}
		Row {
			width : parent.width
			height : 25
			visible : propertyForm.editable
			Controls.TextEditLabelPair {
				id : addField
				labelWidth : 30
				labelText : qsTr("Tag")
				width : 150
				height : parent.height
				anchors.verticalCenter : parent.verticalCenter
				transparentBackgrond : false
			}
			Controls.TextEditLabelPair {
				id : valueField
				labelWidth : 40
				labelText : qsTr("Value")
				width : parent.width - addField.width - addBut.width
				height : parent.height
				anchors.verticalCenter : parent.verticalCenter
				transparentBackgrond : false

			}
			Controls.PushButton {
			id : addBut
				width : 40
				height : 25
				icon.source : "../../images/plus20.png"
				icon.width : 22
				icon.height : 22
				icon.color : "transparent"
				display : AbstractButton.TextBesideIcon
				ToolTip.text: qsTr("Add a new tag to the selected node or create new one")
				ToolTip.delay: 1000
				ToolTip.timeout: 5000
				ToolTip.visible: hovered
				onPressed : {
				   // addMetaTag(addField.content, valueField.content)
				   var lst = meta.model
				   lst.push({"tag" : addField.content, "value": valueField.content})
				   meta.model = null
				   meta.model = lst
				   addField.content = ''  
					valueField.content = ''
				}
			}
			/*Controls.PushButton {
				id : deleteBut
				width : 90
				height : 30
				text : "Delete Tag"
				icon.source : "../../images/minus20.png"
				icon.width : 22
				icon.height : 22
				icon.color : "transparent"
				display : AbstractButton.TextBesideIcon
				ToolTip.text: qsTr("deletes a tag ( if allowed)")
				ToolTip.delay: 1000
				ToolTip.timeout: 5000
				ToolTip.visible: hovered
			}*/
		}
	}
}