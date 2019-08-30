import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../../" as Base

Item {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor

    Column {
		width : 200
		height : 110
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
		Row {
			width : parent.width
			height : 22
			spacing : 17
			CheckBox {
					id : autozoomid
					width : 80
					height : 20
					text : qsTr("Auto zoom")
					style : Base.CheckBoxStyle1{}
		
			}
			Controls.TextEditLabelPair{
				id : pixelfield
				labelText : qsTr("")
				labelWidth : 1
				height : 20
				width : 60
				visible : autozoomid.checked
				content : "100"

				Text {
					anchors.left : pixelfield.right
					anchors.leftMargin : 4
					x : 3
					width : 60
					height : 20
					text : "Pixels"
				}
			}
		}

		Button {
			width : 150
			height : 22
			text : qsTr("Apply")

			onClicked : {
				var pixZoom = parseInt((autozoomid.checked && pixelfield.content > 0)? pixelfield.content : -1,10)
			    var pix = editor.screenPixel(parseFloat(columnField.content) - 0.5, parseFloat(rowField.content) - 0.5, pixZoom)
				
				var a = layerview.activePanel();
				var mp = a.mapArea()
				var drw = mp.drawer()
				var pnt = drw.mapToGlobal(pix.x, pix.y)
				
				editor.moveCursor(pnt.x, pnt.y) 
			}
		}
    }
}

