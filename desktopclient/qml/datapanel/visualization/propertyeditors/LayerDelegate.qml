import QtQuick 2.2
import QtQuick.Controls 1.5 
import QtQuick.Controls 2.0 as QC20
import QtQml.Models 2.2
import UIContextModel 1.0
import LayerManager 1.0
import LayerModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Row {
	width : 250
	height : 20
	spacing : 3

	function cactive() {
		var nodeid = styleData.value.nodeid
		var layer = layerview.manager.findLayer(nodeid)
		return layer.active
	}

	CheckBox{
		y : 2
		id : visibilityCheck
		width : 17
		height: 17
		checked : cactive()
		opacity : 0.6
		visible : styleData.value && styleData.value.nodeid != 0

		onCheckedChanged : {
			var nodeid = styleData.value.nodeid
			var layer = layerview.manager.findLayer(nodeid)
			layer.vproperty("active", visibilityCheck.checked)
		}


	}
	Image {
		y : 2
		id : image
			width : 16; height :16
		source : styleData.value.icon ? uicontext.ilwisFolder + "/qml/images/" + styleData.value.icon : ""
		fillMode: Image.PreserveAspectFit
		opacity : 0.5
	}						 
    Text {
		y : 2
        elide: styleData.elideMode
        text: styleData.value ? styleData.value.name : ""
		font.bold : styleData.value ? styleData.value.readonly : false
        height : 18
        width : 150
    } 
}