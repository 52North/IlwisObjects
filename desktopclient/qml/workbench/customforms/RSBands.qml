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
	property var selectedBands : []
	Text {
		width : 150
		height : 20
		text : qsTr("Band indexes")
	}
	ScrollView {
		width : parent.width
		height : 80
		ListView {
			id : list
			model : raster.layerInfo
			clip : true

			onModelChanged : {
				selectedBands = []
				for(var i=0; i < model.length; ++i)
					selectedBands.push(false)
			}
			delegate : Row {
						width : parent.width
						height : 20
						spacing : 4
						CheckBox {
							width : 20
							height : 20
							onCheckedChanged : {
								selectedBands[index] = checked
							}
						}
						Text {
							width : 120
							text : list.model[index].index
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
				var bands = "" 
				for(var i=0; i < selectedBands.length; ++i){
					if (  selectedBands[i] == true) {
						if ( bands != "" )
							bands += ","
						bands += list.model[i].index
					}
				}
				bands = "rasterbands(" + bands + ")"
				addRule({ rule : bands, connector : "with"})
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