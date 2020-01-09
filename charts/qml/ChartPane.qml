import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import QtQuick 2.5
import QtCharts 2.1
import ChartModel 1.0
import DataseriesModel 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls

Rectangle {
    id : chartView
    width : parent.width
    height : parent.height - (chartspanel.showManager ? 270 : 0)
    color : "blue"
	property ChartModel chart : chartspanel.chart
    property var updateChart : chart ? chart.updateSeries : 0


	signal click(int mx,int my, var object)

    onUpdateChartChanged : {
        visibleGraphs.removeAllSeries()
        loadGraphs()    
    }

	onChartChanged : {
		loadGraphs();
	}

    Connections {
        target : chart
        onYAxisChanged : {
            if (chart) {
                if (chart.niceNumbersY) {
                    yas.applyNiceNumbers()
                }
            }
        }
    }

    Controls.FloatingRectangle{
        id : floatrect
    }

	ValueAxis {
		id : xas
		min : chart != null ? chart.minX : 0
		max : chart != null  ? chart.maxX : 5
		tickCount : chart ? chart.tickCountX : 5
        labelFormat : chart ? chart.formatXAxis : "%.3f"
        visible : chart ? chart.xAxisVisible : true
	}

	ValueAxis {
		id : yas
		min : chart != null  ? chart.minY : 0
		max : chart != null  ? chart.maxY : 5
		tickCount : chart ? chart.tickCountY : 5
        labelFormat : chart ? chart.formatYAxis : "%.3f"
        visible : chart ? chart.yAxisVisible : true
	}

    CategoryAxis {
        id : itemaxisx
        labelsPosition : CategoryAxis.AxisLabelsPositionOnValue
//        labelsAngle : 45    // from positive x-axis clockwise; in degrees
        visible : chart ? chart.xAxisVisible : true
    }

    CategoryAxis {
        id : itemyaxis
        visible : chart ? chart.yAxisVisible : true
    }

    LinearGradient {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#EBF0EC" }
            GradientStop { position: 1.0; color: "white" }
        }
    }

	ChartView {
		id : visibleGraphs
		title: "Line"
		anchors.fill: parent
		antialiasing: true
        backgroundColor : "transparent"
        theme : ChartView.ChartThemeBlueIcy
        dropShadowEnabled : true
        legend.visible : chart ? chart.legendVisible : true
        legend.alignment : chart ? aligntoint(chart.legendAlignment) : aligntoint("top")

		   Canvas {
				id : canvas
				property var lastPoint

				anchors.fill : parent

				onPaint : {
					var ctx = getContext("2d")
					ctx.reset();
					ctx.beginPath()
					ctx.clearRect(0, 0, width, height);
					ctx.stroke();
					antialiasing = false
					var curOper = propertiespanel.currentEditor()
					if (typeof curOper.handleUI === "function"){
						curOper.handleUI(visibleGraphs, canvas)
					}
				}
				z : 5
		   }

		   MouseArea {
            id : chartMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
			hoverEnabled: true

			onPressed : {
				click(mouseX, mouseY,visibleGraphs)
				if ( activeToolBar().zoomMode){
					var newPoint = mapToItem(chartspanel, mouseX, mouseY)
					zoomRectangle.x = newPoint.x
					zoomRectangle.y = newPoint.y
					zoomRectangle.visible = true
				} else if ( visibleGraphs.count > 0){
					info(mouse.x, mouse.y)
					canvas.lastPoint = Qt.point(mouseX, mouseY)
					sendToLink(mouse.x, mouse.y)
					canvas.requestPaint()
				}
			}
			onReleased : {
				floatrect.enabled = false
				floatrect.opacity = 0
				floatrect.x = 0
				floatrect.y = 0
				if ( activeToolBar().zoomMode){
					var newRect = chartspanel.mapToItem(visibleGraphs, zoomRectangle.x, zoomRectangle.y,zoomRectangle.width, zoomRectangle.height)
					visibleGraphs.zoomIn(newRect) 
					zoomRectangle.disable()
				}
			}

			onPositionChanged : {

				if ( activeToolBar().zoomMode && zoomRectangle.visible){
					var newPoint = mapToItem(chartspanel, mouseX, mouseY)
					if ( newPoint.x < zoomRectangle.x)
						zoomRectangle.x = newPoint.x
					else if ( newPoint.x > zoomRectangle.x)
						zoomRectangle.width = newPoint.x - zoomRectangle.x
					if ( newPoint.y < zoomRectangle.y)
						zoomRectangle.y = newPoint.y
					else if ( newPoint.y > zoomRectangle.y)
						zoomRectangle.height = newPoint.y - zoomRectangle.y
				}else if ( floatrect.opacity > 0){
					info(mouse.x, mouse.y)
					canvas.lastPoint = Qt.point(mouseX, mouseY)
					sendToLink(mouse.x, mouse.y)
					canvas.requestPaint()
		        }
			}
		}

        DropArea {
            anchors.fill: parent

            onDropped: {
                    chart.addDataTable(drag.source.ilwisobjectid);
            }
        }
    }

	function aggregate(points) {
        var accu = {};

		for (var j = 0; j < points.length; j++) {
			var key = points[j].x;
			if (key in accu)
				accu[key] += points[j].y;
			else
				accu[key] = points[j].y;
		}

		return accu;
	}

	function loadGraphs() {
        if ( !chart)
            return

        var currxaxis = xas;
        var xaxistype = chart.xaxisType;
		var labels = []
        if (xaxistype == 3) {   // 3 == chart.AxisType.AT_CATEGORIES
            var ser = chart.getSeries(0)
            var xcat = ser.categories("xaxis", true);
            var keys = xcat.keys;
            labels = xcat.labels;
            currxaxis = [];
            itemaxisx.min = ser.minID - 1;
            itemaxisx.max = ser.maxID + 1;
            for (var i = 0; i < keys.length; i++) {
                itemaxisx.append(labels[i], i); //keys[i]);
            }
            currxaxis = itemaxisx
        }

		for (var i = 0; i < chart.seriesCount; i++) {
			var smodel = chart.getSeries(i);
            var ctype = smodel.charttype
            var series = createSeries(ctype, smodel.name, currxaxis, yas)
            if (ctype == "line" || ctype == "spline" || ctype == "points") {
			    series.pointsVisible = false;
			    series.color = chart.seriesColor(i);
			    var points = smodel.points
			    var pointsCount = points.length;
			    for (var j = 0; j < pointsCount; j++) {
				    series.append(points[j].x, points[j].y);
			    }
            }
            if (ctype == "bar") {
				var accu = aggregate(smodel.points);

				var yvals = []
				var ymin = accu[0]
				var ymax = accu[0]
			    for (var j = 0; j < labels.length; j++) {
                    var id = keys[j];
					yvals.push(accu[id])
					ymin = Math.min(ymin, accu[id])
					ymax = Math.max(ymin, accu[id])
				}
				yas.min = ymin
				yas.max = ymax
			    var bar = series.append(smodel.name, yvals);
                bar.color = chart.seriesColor(i);
				series.labelsPosition = AbstractBarSeries.LabelsInsideEnd
				series.labelsVisible = true
            }
            if (ctype == "pie") {
				var accu = aggregate(smodel.points);

                for (var j = 0; j < labels.length; j++) {
                    var cat = labels[j];
                    var id = keys[j];
                    var slice = series.append(cat, accu[id]);
                    slice.labelVisible = true
                }
            }
		}
	}

    function aligntoint(align) {
        if (align == "top")
            return 0x20
        if (align == "right")
            return 0x02
        if (align == "bottom")
            return 0x040
        if (align == "left")
            return 0x01

        return 0x20 // fallback
    }
	function sendToLink(mx,my){
		var pnt = visibleGraphs.mapToValue(Qt.point(mx,my),visibleGraphs.series(0))
		var curOper = propertiespanel.currentEditor()
		if (typeof curOper.handleClick === "function"){
			curOper.handleClick(visibleGraphs, mx, my)
		}
	}
	function info(mx,my){
		var pnt = visibleGraphs.mapToValue(Qt.point(mx,my),visibleGraphs.series(0))
		var text = pnt.x.toFixed(3) + " " + pnt.y.toFixed(3)
		floatrect.enabled = true
		floatrect.opacity = 1
		floatrect.x = mx
		floatrect.y = my
		floatrect.text = text
	}

    function createSeries(ctype, name, xas, yas){
        if ( ctype == "line") {
            return visibleGraphs.createSeries(ChartView.SeriesTypeLine, name, xas, yas);
        }
        if (ctype == "spline") {
            return visibleGraphs.createSeries(ChartView.SeriesTypeSpline, name, xas, yas);
        }
        if (ctype == "bar")
            return visibleGraphs.createSeries(ChartView.SeriesTypeBar, name, xas, yas);
        if ( ctype == "pie")
            return visibleGraphs.createSeries(ChartView.SeriesTypePie, name);
        if ( ctype == "points") {
            var series =  visibleGraphs.createSeries(ChartView.SeriesTypeScatter, name, xas, yas);
            series.markerSize = 10
            return series
        }
        if ( ctype == "polar")
            return visibleGraphs.createSeries(ChartView.SeriesTypeScatter, name, xas, yas);
        if ( ctype == "area")
            return visibleGraphs.createSeries(ChartView.SeriesTypeArea, name, xas, yas);
        return visibleGraphs.createSeries(ChartView.SeriesTypeLine, name, xas, yas);
    }

	function zoomReset() {
		visibleGraphs.zoomReset()
	}
}