import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import QtQuick.Controls 2.3 as QC2
import OperationModel 1.0
import MasterCatalogModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Column {
	x : 4
	spacing : 4
	property var selectedAttributes : []
	ScrollView {
		width : parent.width
		height : 80
		ListView {
			id : list
			model : raster.attributes
			clip : true

			onModelChanged : {
				selectedAttributes = []
				for(var i=0; i < model.length; ++i)
					selectedAttributes.push(false)
			}
			delegate : Row {
						width : parent.width
						height : 20
						spacing : 4
						CheckBox {
							width : 20
							height : 20
							onCheckedChanged : {
								selectedAttributes[index] = checked
							}
						}
						Text {
							width : 120
							text : attributename
						}
					}
		}
	}
	Row {
		spacing : 4
		Controls.PushButton {
			width : 100
			height : 20
			text : qsTr("apply")
			onClicked : {
				var attribs = "" 
				for(var i=0; i < selectedAttributes.length; ++i){
					if (  selectedAttributes[i] == true) {
						if ( attribs != "" )
							attribs += ","
						attribs += list.model[i].attributename
					}
				}
				attribs = "attributes(" + attribs + ")"
				addRule({ rule : attribs, connector : "and"})
				closeMe()
			}
		}
		Controls.PushButton {
				width : 100
				height : 20
				text : qsTr("Cancel")

				onClicked : {
					closeMe()
				}
		}
	}
}