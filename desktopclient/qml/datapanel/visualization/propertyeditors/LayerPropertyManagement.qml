import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Rectangle {
    id : propertyEditorBar

    property var renderer
    property int layerIndex : 0
    color : Global.actionItemColor
    property string iconName : "../images/display_options"
	//property alias treeModel : layerColumn.treeModel
	function setModel(){
		layerColumn.setModel()
	}

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../../images/" + name
         return iconP
     }


    SplitView {
        width : parent.width - 5
        height : parent.height
        y : 2
		id : layerProperties

//        handleDelegate: Controls.SplitHandle{
//            imageHeight: 15
//        }
		function initColumnWidth(name){
			if ( name == "layerlist"){
				return width - attributeListColumn.width - editorListColumn.width - editorColumn.width
			}
			if ( name == "visualattributelist"){
				return width - layerColumn.width - editorListColumn.width - editorColumn.width
			}
			if ( name == "visualattributeeditorlist"){
				return width - layerColumn.width - attributeListColumn.width - editorColumn.width
			}
			if ( name == "editorcolumn"){
//				console.debug("uuuu",viewmanager.width, layerProperties.width, layerColumn.width,  attributeListColumn.width,editorListColumn.width)

				return layerProperties.width - layerColumn.width - attributeListColumn.width - editorListColumn.width
			}
			return 0
		}

        LayersList{
            id : layerColumn
            height : parent.height
            width : 300
            anchors.left: parent.left


        }

        VisualAttributeList{
            id : attributeListColumn
            height : parent.height - 3
            width : 120
        }

        VisualAttributeEditorList{
            id : editorListColumn
            height : parent.height - 3
            width : 120
            x: 4
        }
        VisualAttributeEditor{
            height : parent.height - 3
            width : layerProperties.initColumnWidth("editorcolumn")
            id : editorColumn
        }
    }
}





