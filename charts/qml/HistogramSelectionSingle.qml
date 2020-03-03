import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml" as Base
import ChartModel 1.0
import DataseriesModel 1.0

Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation
	property var lastSelection : "" 
	property var selColor : "#ff0000"
	property var lastPnt
	property var selectedX
	property var chartCanvas
	property var currentValue : operation.currentValue

	onCurrentValueChanged : {
		lastSelection = "at"
		activeSelect.checked = true
		lastPnt = Qt.point(currentValue, 0)
		chart.sendOverLink({"type" : "histogramselection", selectionmode : "at" , "x" : currentValue, "y" : 0,"color"  : selColor })
		var pnt = chart.view().mapToPosition(Qt.point(currentValue,0),chart.view().series(0)) 
		selectedX = pnt.x
		chart.view().requestPaint()
	}

	function handleClick(chartview, mx,my, mode){
		if ( mode == "released")
			return
		var pnt = chartview.mapToValue(Qt.point(mx,my),chartview.series(0))
		if ( lastSelection != "") {
			lastPnt = pnt
			selectedX = mx
			chart.sendOverLink({"type" : "histogramselection", selectionmode : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color"  : selColor })
		}
	}

	function handleUI(chartview, pcanvas) {
	
		if ( lastPnt != null && lastSelection != "" ) {
			chartCanvas = pcanvas
			var ctx = pcanvas.getContext("2d")
			var minx = chart.minX
			var maxx = chart.maxX
			var miny = chart.minYLeft
			var maxy = chart.maxYLeft
			var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
			var pnt3 = chartview.mapToPosition(Qt.point(maxx,maxy),chartview.series(0))
			if ( pnt1.x < selectedX && pnt3.x > selectedX) {
				ctx.beginPath()
				ctx.strokeStyle = "black"
				ctx.moveTo(selectedX, pnt1.y)
				ctx.lineTo(selectedX, pnt2.y)
				ctx.stroke()
			}
		}
	}

	Column {
	   height : 160
	   width : parent.width
	   x : 5
	   Row {
			width : parent.width
			height : 22
		    CheckBox {
				id : activeSelect
				width : 120
				height : 20
				text : qsTr("Activate Selections")

				onClicked : {
					if (!checked){
						lastSelection = ""
						if ( chartCanvas) {
							chartCanvas.requestPaint()
							chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						}
					}else{
						// clear all selections
						if( chartCanvas){
							lastPnt = null
							chartCanvas.requestPaint()
						}
						chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						lastSelection = "at"
					}
				}
			}
			Controls.ColorPicker2{
				id : selectColor
				width : Math.min(250, parent.width * 0.8)
				visible : activeSelect.checked
				onSelectedColorChanged: {
					selColor = selectedColor
					chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
				}
				z : 100
			}
		}

		ExclusiveGroup { id: tabPositionGroup }
	Column {
		x : 15
		//height : 80
		visible : activeSelect.checked
			ExclusiveGroup { id: tabPositionGroup2 }
			RadioButton {
				id : b21
				text: "Above"
				exclusiveGroup: tabPositionGroup2
				checked : lastSelection == "above"

				onClicked: {
					lastSelection = "above"
					if ( lastPnt)
						chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
				}
			}
			RadioButton {
				id : b22
				text: "Below"
				exclusiveGroup: tabPositionGroup2
				checked : lastSelection == "below"

				onClicked: {
					lastSelection = "below"
					if ( lastPnt)
						chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
					}
			}
			RadioButton {
				id : b23
				text: "At"
				exclusiveGroup: tabPositionGroup2
				checked : lastSelection == "at" || lastSelection == ""
				onClicked: {
					lastSelection = "at"
					if ( lastPnt)
						chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
				}
			}
		}
	}
}

