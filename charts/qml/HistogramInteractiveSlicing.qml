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
	property var chartCanvas

	function handleClick(chartview, mx,my){
		/*var pnt = chartview.mapToValue(Qt.point(mx,my),chartview.series(0))
		if ( lastSelection != "") {
			lastPnt = pnt
			chart.sendOverLink({"selectionmode" : lastSelection , "x" : lastPnt.x, "y" : lastPnt.y,"color" : selColor})
		}*/
	}

	function handleUI(chartview, canvas) {
		/*if ( canvas.lastPoint != null && lastSelection != "" ) {
			chartCanvas = canvas
			var ctx = canvas.getContext("2d")
			var minx = chart.minX
			var maxx = chart.maxX
			var miny = chart.minY
			var maxy = chart.maxY
			var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
			var pnt3 = chartview.mapToPosition(Qt.point(maxx,maxy),chartview.series(0))
			var color = Qt.rgba(255,0,0,1)
			if ( pnt1.x < canvas.lastPoint.x && pnt3.x > canvas.lastPoint.x) {
				ctx.beginPath()
				ctx.strokeStyle = "black"
				ctx.moveTo(canvas.lastPoint.x, pnt1.y)
				ctx.lineTo(canvas.lastPoint.x, pnt2.y)
				ctx.stroke()
			}
		}*/
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
					}else
						lastSelection = "at"
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
	}
}

