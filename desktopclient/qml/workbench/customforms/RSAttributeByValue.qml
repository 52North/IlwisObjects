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
	spacing : 4

	Controls.ComboxLabelPair {
	   id : attrList
        labelText :  qsTr("Attribute")
        labelWidth : 100
        role : 'attributename'
        width : parent.width
        height : 20
		itemModel : raster ? raster.attributes : null
	}
	Controls.ComboxLabelPair {
	   id : conditionList
        labelText :  qsTr("Condition")
        labelWidth : 100
        width : parent.width
        height : 20
		itemModel : ["<", "<=","==","!=", ">", ">="]
	}
	Controls.TextEditLabelPair {
	    id : valueEdit
		width : parent.width
		height : 20
		labelText : qsTr("Value")
		labelWidth : 100
	}

	Row {
		spacing : 4
		Controls.PushButton {
			width : 100
			height : 20
			text : qsTr("Apply")

			onClicked : {
				addRule({ rule : attrList.comboText + conditionList.comboText + valueEdit.content, connector : "and"})
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