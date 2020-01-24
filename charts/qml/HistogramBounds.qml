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
	property var selColor : "#880000"
	property var bound
	property var inverse : false
    property var selX
	property var chartCanvas

	function handleClick(chartview, mx,my, mode){
		if ( mode == "released")
			return
		if (!activeSelect.checked)
			return

		var pnt = chartview.mapToValue(Qt.point(mx,my),chartview.series(0))
		bound = pnt	
		selX = mx
		changeSelection()
	}

	function changeSelection(){
		if ( inverse){
			chart.sendOverLink({"type" : "histogrambounds",  "minvalue" :bound.x, "maxvalue" : chart.maxX,"color" :  selColor})
		}else
			chart.sendOverLink({"type" : "histogrambounds",  "minvalue" : chart.minX, "maxvalue" : bound.x,"color" :  selColor})
	}

	function drawStrokedText(ctx, text, x, y) {
		ctx.font = '12px sans-serif';
		ctx.strokeStyle = 'black';
		ctx.strokeText(text, x, y-5);
		ctx.fillStyle = 'black';
		ctx.fillText(text, x, y-5);
	}
	function handleUI(chartview, canvas) {
		if ( bound != null ||  !activeSelect.checked) {
			chartCanvas = canvas
			var ctx = canvas.getContext("2d")
			var minx = chart.minX
			var maxx = chart.maxX
			var miny = chart.minYLeft
			var maxy = chart.maxYLeft
			
			var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
			var pnt3 = chartview.mapToPosition(Qt.point(maxx,maxy),chartview.series(0))
			if ( pnt1.x < selX && pnt3.x > selX) {
				ctx.beginPath()
				ctx.strokeStyle = "blue"
				ctx.moveTo(selX, pnt1.y)
				ctx.lineTo(selX, pnt2.y)
				ctx.stroke()
				ctx.beginPath()
				ctx.strokeStyle = "green"
				var v = operation.yPercValue(bound.x)
				var pnt4 = chartview.mapToPosition(Qt.point(bound.x, v), chartview.series(1))
				ctx.moveTo(pnt4.x, pnt4.y)
				ctx.lineTo(pnt1.x + 4, pnt4.y)
				if ( inverse)
					v = 100 - v
				v = v + ' %'
				drawStrokedText(ctx, v, (selX + pnt1.x)/2, pnt4.y)
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
							chartCanvas.requestPaint()
							chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						}
					}else{
						// clear all selections
						chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						if ( chartCanvas)
							chartCanvas.requestPaint()
					}
				}
			}
			Controls.ColorPicker2{
				id : selectColor
				width : Math.min(250, parent.width * 0.8)
				visible : activeSelect.checked
				onSelectedColorChanged: {
					selColor = selectedColor
					//chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
				}
				z : 100
			}
		}

		Column {
			x : 15
			height : 100
			visible : activeSelect.checked
			spacing : 4

			ExclusiveGroup { id: tabPositionGroup2 }
			CheckBox {
				text : "Inverse"
				onCheckedChanged: {
					changeSelection()
					if ( chartCanvas)
						chartCanvas.requestPaint()
				}
			}
		}
	}
}


