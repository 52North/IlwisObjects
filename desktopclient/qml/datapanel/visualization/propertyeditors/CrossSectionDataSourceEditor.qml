import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ResourceModel 1.0
import "../../../Global.js" as Global
import "../../../Controls" as Controls
import "../../.." as Base


TableView {
	id : items

	TableViewColumn{
		title : qsTr("Active")
		role : "active"
		width : 30
		delegate: CheckBox {
			height : 20
			width : 20
			checked : styleData.value
			style : Base.CheckBoxStyle1{}
			onClicked : {
				editor.setActive(index, checked)
				var chartModel = models.model(modelid)
				if ( chartModel){
					for(var i=0; i < editor.pinCount; ++i){
						var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(i+1) + ")"
						layerview.activeLayerManager().addCommand(expr);
					}
				}
			}
        } 
	}
	TableViewColumn{
		title : qsTr("Name")
		role : "name"
		width : 130
		delegate: Text  {
			height : 20
			width : 120
			text : styleData.value
		}
	}

	TableViewColumn{
	    id : minv
		title : qsTr("Minimum")
		role : "minvalue"
		width : 60
		visible : false

		delegate: Text {
			height : 20
			width : 60
			text : styleData.value
		}
	}

	TableViewColumn{
	    id : maxv
		title : qsTr("Maximum")
		role : "maxvalue"
		width : 60
		visible : false

		delegate : Text {
			height : 20
			width : 60
			text : styleData.value
		}
	}

	rowDelegate: Rectangle {
        id : rowdelegate
        height : 23
        color : styleData.selected ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
    }
	DropArea {
	    id : dropArea
		anchors.fill : parent
		onDropped : {
				minv.visible = false
				maxv.visible = false
			    var obj = mastercatalog.id2object(drag.source.ilwisobjectid, dropArea)
				console.debug("zzzzz", obj.internalValuetype)
				if(obj.internalValuetype == "numericinterval"){
					minv.visible = true
					maxv.visible = true
					console.debug(obj.name)
					editor.setStackDomain(drag.source.ilwisobjectid)
				}

    	}
	}

}