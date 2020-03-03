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
	property var selColor : "#ff0000"
	property var lastPnt
	property var chartCanvas
	property var currentValue : operation.currentValue

	onCurrentValueChanged : {
		activeSelect.checked = true

		lastPnt = Qt.point(currentValue, 0)
		chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "at" , "x" : operation.getValues(currentValue), "y" : 0,"color"  : selColor })
		var pntRed = chart.view().mapToPosition(Qt.point(operation.bandValue("histogram_red"),0),chart.view().series(0)) 
		var pntGreen = chart.view().mapToPosition(Qt.point(operation.bandValue("histogram_green"),0),chart.view().series(0))
		var pntBlue = chart.view().mapToPosition(Qt.point(operation.bandValue("histogram_blue"),0),chart.view().series(0)) 		
		operation.setXs(pntRed.x, pntGreen.x, pntBlue.x)
		chart.view().requestPaint()
	}

	function handleClick(chartview, mx,my, mode){
		if ( mode == "released" || activeSelect.checked == false)
			return
		var pnt = chartview.mapToValue(Qt.point(mx,my),chartview.series(0))

		var minx = chart.minX
		var maxx = chart.maxX
		var miny = chart.minYLeft
		var maxy = chart.maxYLeft
		var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
		var pnt3 = chartview.mapToPosition(Qt.point(maxx,maxy),chartview.series(0))
		if ( pnt1.x < mx && pnt3.x > mx) {
			lastPnt = pnt
			operation.currentX = mx
			chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "at" , "x" : operation.getValues(lastPnt.x), "y" : lastPnt.y,"color"  : selColor })
		}
	}

	function handleUI(chartview, pcanvas) {
	
		var xred =  operation.bandX("histogram_red")
		var xgreen =  operation.bandX("histogram_green")
		var xblue =  operation.bandX("histogram_blue")
		if ( xred !=  -100000 || xblue !=  -100000 || xgreen !=  -100000 ) {
			chartCanvas = pcanvas
			var ctx = pcanvas.getContext("2d")
			var minx = chart.minX
			var maxx = chart.maxX
			var miny = chart.minYLeft
			var maxy = chart.maxYLeft
			var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
			var pnt3 = chartview.mapToPosition(Qt.point(maxx,maxy),chartview.series(0))

			if ( xred != -100000){
				ctx.beginPath()
				ctx.strokeStyle = "red";
				ctx.moveTo(xred, pnt1.y)
				ctx.lineTo(xred, pnt2.y)
				ctx.stroke()
			}
			if ( xgreen != -100000){
				ctx.beginPath()
				ctx.strokeStyle = "green";
				ctx.moveTo(xgreen, pnt1.y)
				ctx.lineTo(xgreen, pnt2.y)
				ctx.stroke()
			}
			if ( xblue != -100000){
				ctx.beginPath()
				ctx.strokeStyle = "blue";
				ctx.moveTo(xblue, pnt1.y)
				ctx.lineTo(xblue, pnt2.y)
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
						if ( chartCanvas) {
							lastPnt = null
							operation.currentX = -100000
							operation.currentValue = 9999999.0
							chartCanvas.requestPaint()
						}
					}else{
						// clear all selections
						if( chartCanvas){
							lastPnt = null
							chartCanvas.requestPaint()
						}
						chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
					}
				}

			}
			Controls.ColorPicker2{
				id : selectColor
				width : Math.min(250, parent.width * 0.8)
				visible : activeSelect.checked
				onSelectedColorChanged: {
					selColor = selectedColor
					chart.sendOverLink({"selectionmode" : "at" , "x" : operation.getValues(lastPnt.x), "y" : lastPnt.y,"color" : selColor})
				}
				z : 100
			}
		}
		Controls.PushButton {
			width : 120
			height : 22
			text : qsTr("Remove stretch")

			onClicked : {
				chart.sendOverLink({"type" : "histogramselectioncc", "resetstretch" : true})
			}
		}
	}
}

