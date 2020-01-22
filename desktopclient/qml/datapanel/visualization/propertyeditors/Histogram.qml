import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Column {
    width: 100
    height: 62
    property var editor
	property var modelid : null
	spacing : 5
	property var currentPolygon : []
	property var editState : false

	function handleMousePressed(mx,my) {
		if ( useAOI.checked && editState)
			editor.addPoint(mx,my)	
	}

    Button{
        id : usesInfo
        x : 5
        y : 5
        text: "Open Histogram"
		width : 100
		height : 22

		onClicked : {
			  var mid = editor.visualAttribute.layer.modelId()
			  var createInfo
			  if ( editor.aggregateAOIs)
  				createInfo = {type : "chart", url : editor.tableUrl, ctype : 'line', name : editor.editorName , xaxis : 'min', yaxis :'histogram|-histogram_cumulative', zaxis : '', extraparameters : 'resx=2|resy=2|specialtype=histogram|linkedid=' + mid}
			  else{
				var ydata = "" 
				for(var i=0; i < editor.polyCount; ++i) {
					if ( ydata != "")
						ydata += "|"
					ydata += 'histogram_' + i + '|-histogram_cumulative_' + i
				}
				createInfo = {type : "chart", url : editor.tableUrl, ctype : 'line', name : editor.editorName , xaxis : 'min', yaxis : ydata, zaxis : '', extraparameters : 'resx=2|resy=2|specialtype=histogram|linkedid=' + mid}
			  }
              modelid = objectcreator.createObject(createInfo)
			  editor.chartModelId = modelid
              var filter = "itemid=" + modelid
              bigthing.newCatalog(filter, "chart", "","other")
		}
    }

	Column {
		x : 4
		spacing : 4
		CheckBox {
			id : useAOI
			height : 22
			width : 120
			text : qsTr("Use Area of interest")
			checked : false

			onCheckedChanged : {
				editor.useAOI = checked
			}
			CheckBox{
				id : aggrAOI
				height : 22
				width : 120
				text : qsTr("Aggregate Areas of interest")
				checked : true

				onCheckedChanged : {
					editor.aggregateAOIs = checked
				}
				anchors.left : useAOI.right
				anchors.leftMargin : 8
			}
		}
		Controls.PushButton {
			text : editState ? qsTr("End current AOI") : qsTr("Add new AOI")
			width : 140
			height : 22
			visible : useAOI.checked
			checkable : true
			checked : editState
			highlighted : editState

			onClicked : {
				if (!editState)
					editor.addEmptyPolygon()
				editState = !editState
			}
		}

		Controls.PushButton {
			text : qsTr("Delete last AOI's")
			width : 140
			height : 22
			visible : useAOI.checked

			onClicked : {
				editState = false
				editor.deleteLastAOI()
			}
		}

		Controls.PushButton {
			text : qsTr("Delete all AOI's")
			width : 140
			height : 22
			visible : useAOI.checked

			onClicked : {
				editState = false
				editor.deleteAllAOIs()
			}
		}
	}


}

