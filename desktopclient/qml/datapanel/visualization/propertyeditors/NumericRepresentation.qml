import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import RepresentationElement 1.0
import VisualAttribute 1.0
import LayerModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width : displayEditorColumn.width - 20
    height : 150
    y : 4
     property var editor
	 DropArea {
		id : dropArea
		anchors.fill : parent
	
		Column {
			spacing : 4
			width : parent.width
			Row {
				height : 60
				width : parent.width
				Controls.LegendBar {
					id : bar
					width : Math.min(350, parent.width)
					x : 10
					height : 60
					items : editor.representationElements
					property var currentException : -1
					property var bandWidth : 5

					onItemsChanged : {
						bar.requestPaint()
					}
					colorFunc : editor.color

					MouseArea {
						x : 0
						y : 19
						anchors.fill : parent
						onClicked : {
							if ( select.checked){
								if ( mouseX > 0) {
									bar.currentException = mouseX /  (bar.width - 15)
									var p = parseFloat(bar.currentException)
									editor.addException(p, bar.bandWidth/100.0, colorid.selectedColor)
								}
							}
						}
					}
				}
				TextField {
					width : 60
					height : 20
					y : 20
					visible : select.checked
					text : (bar.currentException * (bar.max() - bar.min())).toFixed(2)
				}
			}
			Row {
				spacing : 6
				CheckBox {
					height : 20
					id : select
					text : qsTr("Select pixels")

					onCheckedChanged : {
						if ( !checked ){
							editor.addException(-1, 0, colorid.selectedColor) // clears any selection
						}
					}
				}
				Controls.ColorPicker2{
					id : colorid
					width : Math.min(250, parent.width * 0.8)
					x : 14
					y : 4
					visible : select.checked
					dropdownheight : 150
					initialColor : "#ff0000"

					onSelectedColorChanged: {
						if ( bar.currentException > 0) {
							editor.addException(parseFloat(bar.currentException), bar.bandWidth/100.0, colorid.selectedColor)
						}
					}
					z : 100
				}
			}
			Controls.TextEditLabelPair {
				width : 250
				labelText : "Band width(%)"
				labelWidth : 100
				content : parseInt(bar.bandWidth)
				visible : select.checked

				onContentChanged : {
						if ( content != "" ) {
							bar.bandWidth = parseInt(content)
							if ( bar.currentException > 0 && bar.bandWidth > 0) {
								editor.addException(parseFloat(bar.currentException), bar.bandWidth/100.0, colorid.selectedColor)
							}
						}
				}
			}

	
		}

		onDropped : {
			if ( 'ilwisobjectid' in drag.source){
				var obj = mastercatalog.id2object(drag.source.ilwisobjectid, dropArea)
				if(obj.typeName == "representation"){
					editor.visualAttribute.layer.vproperty("visualattribute|" + editor.visualAttribute.attributename + "|representation", obj.url)
					bar.requestPaint()
				}
			}
		}
	}

}
