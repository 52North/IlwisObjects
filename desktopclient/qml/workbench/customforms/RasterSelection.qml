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
	id : container
	width : parent.width
	height : 320
	spacing : 4
	property var raster
	property var formresult : getFormResult()
	property var connectorVisible : false

	ListModel {
		id: data
	}
	
	Controls.TextEditLabelPair {
		id : inputRaster
		width : parent.width
		height : 20
		labelText : qsTr("Input raster")
		labelWidth : 100
		callbackFunction : setObject
	}

	TableView {
		model : data
		height : 100
		width : parent.width
		TableViewColumn {
			role : "rule"
			title : "Rule"
		}

		TableViewColumn {
			role : "connector"
			title : "connector"

			delegate: ComboBox {
				width : 50
				height : 20
				visible : connectorVisible
				model : determineModel(styleData.row)

				Component.onCompleted : {
					model = ["and", "or"]
					currentIndex = 0
					if (  styleData.value == "or")
						currentIndex = 1
				}
			}
		}

	}
	Row {
		width : parent.width
		height : 24
		Controls.PushButton {
			width : 100
			height : 22
			text : "Create rule for"

			onClicked : {
				if ( rules.currentText == "attribute by value") {
					ruleLoader.source = "RSAttributeByValue.qml"
				}
				if ( rules.currentText == "attributes") {
					ruleLoader.source = "RSAttributes.qml"
				}
				if ( rules.currentText == "bounding box") {
					ruleLoader.source = "RSBoundingBox.qml"
				}
				if ( rules.currentText == "bands") {
					ruleLoader.source = "RSBands.qml"
				}
			
			}
		}
		ComboBox {
			id : rules
			width : 150
			height : 22
		}
	}

	Loader {
		id : ruleLoader
		width : parent.width
		height : 140


	}

	Controls.TextEditLabelPair {
		id : outputRaster
		width : parent.width
		height : 20
		labelText : qsTr("Output raster")
		labelWidth : 100
		callbackFunction : setObject
	}

	Controls.TextEditLabelPair{
		id: outputRes
		labelText:qsTr("Numeric resolution")
		boldLabel : false
		labelWidth:100
		width:200;height:20
		content : getResolution()
	}

	Controls.ComboxLabelPair{ 
		id: outFormat
		//callback : addMRUFormat
		width : parent.width  - 5
		fontBold : false
		labelWidth : 100
		itemModel : mastercatalog.formats("rastercoverage")
		labelText :qsTr("Output format")
	}

	function setObject(url) { 
		raster = mastercatalog.url2object(url, "rastercoverage", container)
		var r =  [ "attribute by value", "bounding box"]
		if ( raster.hasAttributes){
			r.push("attributes")
		}if ( raster.getProperty("bandcount") > 1)
			r.push("bands")
		rules.model = r
	}

	function closeMe() {
		ruleLoader.source = "" 
	}

	function addRule(rule) {
		data.append(rule)
		connectorVisible = data.count > 1
	}

	function getResolution(){
		var resolution = 0
		if ( raster) {
			var numrange = raster.getProperty("numericrange")
			if ( numrange != "?" ){
				var parts = numrange.split("|")
				if ( parts.length == 3){
					resolution = parts[2]
				}
			}
		}
		return resolution
	}
	function getFormResult(){

		var prevConnector = ""
		var result = "" 
		for( var i=0; i< data.count; ++i){
			if ( prevConnector != "")
				result += " " + prevConnector
			result += data.get(i).rule
			prevConnector = data.get(i).connector
		}
	
		var rules =  "\"" +  result + "\"" 
		var resolution = getResolution()
		if ( resolution == 0)
			result =  inputRaster.content + "|" + rules + "|" + outputRaster.content + "@@" +outFormat.comboText
		else
			result =  inputRaster.content + "|" + rules + "|" + outputRaster.content + "@@" +resolution + "@@" + outFormat.comboText
		return result
	}

	function determineModel(idx) {
		var result = []
		if ( data.count > 1 && idx < data.count - 1){
			if ( data.get(idx+1).connector == "andor") {
				result = ["and", "or"]
			}
			if ( data.get(idx+1).connector == "with") {
				result = ["with:"]
			}

		}
		return result
	}

}