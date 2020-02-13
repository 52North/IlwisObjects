import QtQuick 2.11
import QtQuick.Controls 1.1
import QtQuick.Controls 2.5 as QC2
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

Column {
	x : 4
	y:  4
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation
	spacing : -4

	function handleUI(chartview, pcanvas) {
		var markers = operation.markers
		if ( markers.hasOwnProperty("loweroriginal") && stretchtooltid.checked) {
			var ctx = pcanvas.getContext("2d")
			var minx = chart.minX
			var maxx = chart.maxX
			var miny = chart.minYLeft
			var maxy = chart.maxYLeft
			var pnt1 = chartview.mapToPosition(Qt.point(parseFloat(markers["loweroriginal"]),miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(parseFloat(markers["loweroriginal"]),maxy),chartview.series(0))
			ctx.beginPath()
			ctx.strokeStyle = "green"
			ctx.moveTo(pnt1.x, pnt1.y)
			ctx.lineTo(pnt2.x, pnt2.y)
			ctx.stroke()

			var pnt5 = chartview.mapToPosition(Qt.point(parseFloat(markers["upperoriginal"]),miny),chartview.series(0))
			var pnt6 = chartview.mapToPosition(Qt.point(parseFloat(markers["upperoriginal"]),maxy),chartview.series(0))

			ctx.beginPath()
			ctx.strokeStyle = "blue"
			ctx.moveTo(pnt5.x, pnt5.y)
			ctx.lineTo(pnt6.x, pnt6.y)
			ctx.stroke()

			var pnt3 = chartview.mapToPosition(Qt.point(parseFloat(markers["lowermapped"]),miny),chartview.series(0))
			var pnt4 = chartview.mapToPosition(Qt.point(parseFloat(markers["lowermapped"]),maxy),chartview.series(0))
			var pnt7 = chartview.mapToPosition(Qt.point(parseFloat(markers["uppermapped"]),miny),chartview.series(0))
			var pnt8 = chartview.mapToPosition(Qt.point(parseFloat(markers["uppermapped"]),maxy),chartview.series(0))

			ctx.beginPath()
			ctx.strokeStyle = "green"
			ctx.moveTo(pnt1.x + 7, pnt2.y + 30)
			ctx.lineTo(pnt3.x - 10, pnt2.y + 30)
			ctx.fillStyle = "green"
			ctx.fillRect(pnt3.x-12, pnt2.y + 27, 7,7)
			ctx.stroke()

			ctx.beginPath()
			ctx.moveTo(pnt5.x + 7, pnt6.y + 30)
			ctx.lineTo(pnt7.x + 10, pnt6.y + 30)
			ctx.fillStyle = "blue"
			ctx.fillRect(pnt7.x + 12, pnt6.y + 27, 7,7)
			ctx.stroke()

			ctx.beginPath()
			ctx.strokeStyle = "green"
			ctx.setLineDash([5,15])
			ctx.moveTo(pnt3.x, pnt3.y)
			ctx.lineTo(pnt4.x, pnt4.y)
			ctx.stroke()

			ctx.beginPath()
			ctx.strokeStyle = "blue"
			ctx.setLineDash([5,15])
			ctx.moveTo(pnt7.x, pnt7.y)
			ctx.lineTo(pnt8.x, pnt8.y)
			ctx.stroke()

			ctx.setLineDash([]);




		}
	}

	Controls.LabeledCheckBox {
		id : stretchtooltid
		x : 4
		y : 4
		labelText : qsTr("Activate")
		width : parent.width - 100
		labelWidth : 80
		bold : false

		onCheckedChanged : {
			chart.sendOverLink({"type" : "histogramstretchbounds", "resetstretch" : true})
			if ( slider2.value != slider1.value && stretchtooltid.checked){
				chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : true, "oldvalue" : slider1.currentValue,"newvalue"  : slider2.currentValue })
				chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : true, "oldvalue" : slider1.currentValue,"newvalue"  : slider2.currentValue })
				chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : false, "oldvalue" : slider3.currentValue,"newvalue"  : slider4.currentValue })
				chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : false, "oldvalue" : slider3.currentValue,"newvalue"  : slider4.currentValue })
			}
			if ( stretchtooltid.checked){
				operation.addIntensityCurve()
			}else{
				operation.removeIntensityCurve()
				chart.view().requestPaint()
			}
		}
		Component.onCompleted : {
		}
	}
	Text {
		text : "Lower"
		font.bold : true
		visible : stretchtooltid.checked
	}
	SliderRow {
		id : slider1
		width : Math.min(parent.width, 270)
		height :40
		minV : operation.actualMin
		maxV : operation.actualMax
		labelText : "original"
		startValue : operation.actualMin
		
		onCurrentValueChanged : {
			chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : true, "oldvalue" : slider1.currentValue,"newvalue"  : slider2.currentValue })
			operation.stretchHistogram(true,  slider1.currentValue, slider2.currentValue);
			chart.view().requestPaint()
		}
	}

	SliderRow {
		id : slider2
		width : Math.min(parent.width, 270)
		height :40
		minV : operation.actualMin
		maxV : operation.actualMax
		minValue : slider1.currentValue
		startValue : operation.actualMin
		labelText : "shifted"

		onCurrentValueChanged : {
			chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : true, "oldvalue" : slider1.currentValue,"newvalue"  : slider2.currentValue })
			operation.stretchHistogram(true,  slider1.currentValue, slider2.currentValue);
			chart.view().requestPaint()
		}
	}
	Text {
		text : "Upper"
		font.bold : true
		visible : stretchtooltid.checked
	}
	SliderRow {
		id : slider3
		width : Math.min(parent.width, 270)
		height :40
		minV : operation.actualMin
		maxV : operation.actualMax
		startValue : operation.actualMax
		labelText : "original"

		onCurrentValueChanged : {
			chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : false, "oldvalue" : slider3.currentValue,"newvalue"  : slider4.currentValue })
			operation.stretchHistogram(false,  slider3.currentValue, slider4.currentValue);
			chart.view().requestPaint()
		}
	}
	SliderRow {
		id : slider4
		width : Math.min(parent.width, 270)
		height :40
		minV : operation.actualMin
		maxV : operation.actualMax
		minValue : slider3.currentValue
		startValue : operation.actualMax
		labelText : "shfited"

		onCurrentValueChanged : {
			chart.sendOverLink({"type" : "histogramstretchbounds", "lower" : false, "oldvalue" : slider3.currentValue,"newvalue"  : slider4.currentValue })
			operation.stretchHistogram(false,  slider3.currentValue, slider4.currentValue);
			chart.view().requestPaint()
		}
	}

	function handleReleased(val) {
		operation.updateTextures()
	}
}

