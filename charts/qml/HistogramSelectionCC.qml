import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import QtQuick.Controls 2.5 as QC2
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
	property var selColor : "#000000"
	property var chartCanvas
	property var lastSelection : "at"  
	property var currentValue : operation.currentMin

	onCurrentValueChanged : {
		activeSelect.checked = true

		if ( !operation.useRange){
			//chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "at" , "x" : operation.getValues(currentValue), "y" : 0,"color"  : selColor })
			chart.view().requestPaint()
		}
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
			if (operation.useRange){
				var xpmin = operation.currentMin != 9999999.0 ? chartview.mapToPosition(Qt.point(operation.currentMin,chart.minY),chartview.series(0)) : mx
				var xpmax = operation.currentMin != 9999999.0  ? chartview.mapToPosition(Qt.point(operation.currentMax,chart.minY),chartview.series(0)) : mx
				if ( Math.abs(xpmin - mx) < 4)
					operation.currentMin = pnt.x
				else ( Math.abs(xpmax - mx) < 4)
					operation.currentMax = pnt.x
			}
			else
				operation.currentMin = pnt.x

			chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : lastSelection , "x" : operation.getValues(), "y" : my,"color"  : selColor })
		}
	}

	function drawLimitLine(ctx, chartview,evalue, ecolor){
		if ( evalue != 9999999.0){
			var pnt1 = chartview.mapToPosition(Qt.point(evalue,chart.minYLeft),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(evalue,chart.maxYLeft),chartview.series(0))
			ctx.beginPath()
			ctx.strokeStyle = ecolor;
			ctx.moveTo(pnt1.x, pnt1.y)
			ctx.lineTo(pnt2.x, pnt2.y)
			ctx.stroke()
		}
	}
	function handleUI(chartview, pcanvas) {
		chartCanvas = pcanvas
		var ctx = pcanvas.getContext("2d")
		drawLimitLine(ctx,chartview, operation.bandMinValue("histogram_red"), "red")
		drawLimitLine(ctx,chartview, operation.bandMinValue("histogram_green"), "green")
		drawLimitLine(ctx,chartview, operation.bandMinValue("histogram_blue"), "blue")

		if ( operation.useRange){
			drawLimitLine(ctx,chartview, operation.bandMaxValue("histogram_red"), "red")
			drawLimitLine(ctx, chartview,operation.bandMaxValue("histogram_green"), "green")
			drawLimitLine(ctx, chartview,operation.bandMaxValue("histogram_blue"), "blue")
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
							operation.currentMin = 9999999.0
							operation.currentMax = 9999999.0
							chartCanvas.requestPaint()
							chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						}
					}else{
						// clear all selections
						if( chartCanvas){
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
					chart.sendOverLink({"selectionmode" : lastSelection , "x" : operation.getValues(), "y" : 0,"color" : selColor})
				}
			}
			z: 100
		}
		Controls.PushButton {
			width : 120
			height : 22
			text : qsTr("Remove stretch")

			onClicked : {
				chart.sendOverLink({"type" : "histogramselectioncc", "resetstretch" : true})
			}
		}

		Row {
			spacing : 4
			width : Math.min(parent.width, 500)
			height : 50
			CheckBox {
				id : cbUseRange
				width : 100
				text : "Use range"
				checked : false

				onCheckedChanged : {
					operation.useRange = checked
					if ( checked){
						if ( operation.currentMin != 9999999.0)
							control.first.value = operation.currentMin
						if ( operation.currentMax !=  9999999.0)
							control.second.value = operation.currentMax
						chart.view().requestPaint()
						chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "range" , "x" : operation.getValues(), "y" : 0,"color"  : selColor })
					}else{
							operation.currentMin = 9999999.0
							operation.currentMax = 9999999.0
							chartCanvas.requestPaint()
							chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
					}
				}
			}
			QC2.RangeSlider {
				id : control
				width : parent.width - 100
				from : chart.minX
				to : chart.maxX
				visible : cbUseRange.checked

				//first.value: operation.currentMin
				//second.value: operation.currentMax

				first.handle: Rectangle {
					x: control.leftPadding + control.first.visualPosition * (control.availableWidth - width)
					y: control.topPadding + control.availableHeight / 2 - height / 2
					implicitWidth: 2
					implicitHeight: 26
					//radius: 13
					color: control.first.pressed ? "#f0f0f0" : "#f6f6f6"
					border.color: "#bdbebf"

					Text {
						y : 32
						anchors.horizontalCenter : parent.horizontalCenter
						text : control.first.value.toFixed(2)
						font.pointSize : 8
					}

				}

				first.onPressedChanged : {
					if ( !control.first.pressed){
						operation.markersConfirmed()

					}
				}

				first.onValueChanged : {
					operation.currentMin = control.first.value
					operation.currentMax = control.second.value
					chart.view().requestPaint()
					chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "range" , "x" : operation.getValues(), "y" : 0,"color"  : selColor })
				}

				second.onPressedChanged : {
					if ( !control.second.pressed){
						operation.markersConfirmed()

					}
				
				}
				second.onValueChanged : {
					operation.currentMin = control.first.value
					operation.currentMax = control.second.value
					chart.view().requestPaint()
					chart.sendOverLink({"type" : "histogramselectioncc", selectionmode : "range" , "x" : operation.getValues(), "y" : 0,"color"  : selColor })
				}

				second.handle: Rectangle {
					x: control.leftPadding+ control.second.visualPosition * (control.availableWidth - width)
					y: control.topPadding + control.availableHeight / 2 - height / 2
					implicitWidth: 2
					implicitHeight: 26
					//radius: 13
					color: control.second.pressed ? "#f0f0f0" : "#f6f6f6"
					border.color: "#bdbebf"

					Text {
						y : -6
						anchors.horizontalCenter : parent.horizontalCenter
						text : control.second.value.toFixed(2)
						font.pointSize : 8
					}
				}
			}
		}
	}
}


