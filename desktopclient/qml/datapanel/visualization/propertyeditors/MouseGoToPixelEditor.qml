import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor

    Column {
		width : 200
		height : 80
		spacing : 4
		Controls.TextEditLabelPair{
		    id : columnField
			labelText : qsTr("Column")
			labelWidth : 100
			width : 150
			content : editor.column
		}
		Controls.TextEditLabelPair{
			id : rowField
			labelText : qsTr("Row")
			labelWidth : 100
			width : 150
			content : editor.row
		}

		Button {
			width : 150
			height : 22
			text : qsTr("Apply")

			onClicked : {
			    var pix = editor.screenPixel(parseFloat(columnField.content), parseFloat(rowField.content))
				var mp = layerview.activePanel().maparea()
				var drw = mp.drawer()
				var pnt = drw.mapToGlobal(pix.x, pix.y)
				editor.moveCursor(pnt.x, pnt.y) 
			}
		}
    }
}

