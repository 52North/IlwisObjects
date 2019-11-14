import QtQuick 2.1
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
	
		/*Controls.TextEditLabelPair {
			function check(id){
				return editor.canUse(id)
			}

			labelWidth : 100
			labelText : qsTr("Representation")
			id : rprName
			width : parent.width - 20
			height : Global.rowHeight
			content : editor.representationName
			x : 10
			checkFunction: check

			onContentChanged: {
				editor.visualAttribute.layer.vproperty("visualattribute|" + editor.visualAttribute.attributename + "|representation", content)
				bar.requestPaint()
			}
		}*/



		Controls.LegendBar {
			id : bar
			anchors.top : rprName.bottom
			width : Math.min(350, parent.width)
			x : 10
			height : 60
			items : editor.representationElements
			colorFunc : editor.color
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
