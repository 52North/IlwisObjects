import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Controls 2.5 as QC2
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml" as Base
import ChartModel 1.0
import DataseriesModel 1.0



Rectangle {
    width: 750
    height: parent ? parent.height - 10 : 0
    property var operation
	property bool updateMarkers : false

	function handleUI(chartview, pcanvas) {
	
		var ctx = pcanvas.getContext("2d")
		var minx = chart.minX
		var maxx = chart.maxX
		var miny = chart.minYLeft
		var maxy = chart.maxYLeft
		var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
		var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
		var pntX1 = chartview.mapToPosition(Qt.point(control.first.value,0),chartview.series(0))
		var pntX2 = chartview.mapToPosition(Qt.point(control.second.value,0),chartview.series(0))
		ctx.beginPath()
		ctx.strokeStyle = "black"
		ctx.moveTo(pntX1.x, pnt1.y)
		ctx.lineTo(pntX1.x, pnt2.y)
		ctx.stroke()

		ctx.beginPath()
		ctx.strokeStyle = "black"
		ctx.moveTo(pntX2.x, pnt1.y)
		ctx.lineTo(pntX2.x, pnt2.y)
		ctx.stroke()
	}

	Row {
		spacing : 4
		width : Math.min(parent.width, 500)
		height : 50
		Text {
			width : 100
			text : "Stretch limits"
		}
		QC2.RangeSlider {
			id : control
			width : parent.width - 100
			from : operation.min
			to : operation.max

			first.value: operation.initStretchMin
			second.value: operation.initStretchMax

			first.handle: Rectangle {
				x: control.leftPadding + control.first.visualPosition * (control.availableWidth - width)
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 26
				implicitHeight: 26
				radius: 13
				color: control.first.pressed ? "#f0f0f0" : "#f6f6f6"
				border.color: "#bdbebf"

				Text {
					anchors.centerIn : parent
					text : control.first.value.toFixed(2)
					font.pointSize : 6
				}

			}

			first.onPressedChanged : {
				if ( !control.first.pressed){
					operation.markersConfirmed()

				}
			}

			first.onValueChanged : {
				var positions = []
				positions.push(control.first.value)
				positions.push(control.second.value)
				operation.setMarkers(positions)
				chart.view().requestPaint()
				//chart.sendOverLink({"type" : "histogramstretchcc", "lower" : control.first.value, "upper" : control.second.value, "band" : operation.band() })
			}

			second.onPressedChanged : {
				if ( !control.second.pressed){
					operation.markersConfirmed()

				}
				
			}
			second.onValueChanged : {
				var positions = []
				positions.push(control.first.value)
				positions.push(control.second.value)
				operation.setMarkers(positions)
				chart.view().requestPaint()
				//chart.sendOverLink({"type" : "histogramstretchcc", "lower" : control.first.value, "upper" : control.second.value, "band" : operation.band() })
			}

			second.handle: Rectangle {
				x: control.leftPadding+ control.second.visualPosition * (control.availableWidth - width)
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 26
				implicitHeight: 26
				radius: 13
				color: control.second.pressed ? "#f0f0f0" : "#f6f6f6"
				border.color: "#bdbebf"

				Text {
					anchors.centerIn : parent
					text : control.second.value.toFixed(2)
					font.pointSize : 6
				}
			}
		}
	}
		
}


