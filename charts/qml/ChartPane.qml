import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import QtQuick 2.5
import QtCharts 2.1
import ChartModel 1.0
//import ChartEnums 1.0
import DataseriesModel 1.0

Rectangle {
    id : chartView
    width : parent.width
    height : parent.height - 270
    color : "blue"
	property ChartModel chart : chartspanel.chart
    property var updateChart : chart ? chart.updateSeries : 0

    onUpdateChartChanged : {
        visibleGraphs.removeAllSeries()
        loadGraphs()    
    }

	onChartChanged : {
		loadGraphs();
	}

	ValueAxis {
		id : xas
		min : chart != null ? chart.minX : 0
		max : chart != null  ? chart.maxX : 5
		tickCount : chart ? chart.tickCountX : 5
        labelFormat : chart ? chart.formatXAxis : "%.3f"
	}

	ValueAxis {
		id : yas
		min : chart != null  ? chart.minY : 0
		max : chart != null  ? chart.maxY : 5
		tickCount : chart ? chart.tickCountY : 5
        labelFormat : chart ? chart.formatYAxis : "%.3f"
	}

    CategoryAxis {
        id : itemaxisx
        labelsPosition : CategoryAxis.AxisLabelsPositionOnValue
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

        DropArea {
            anchors.fill: parent

            onDropped: {
                    chart.addDataTable(drag.source.ilwisobjectid);
            }
        }
    }

	function loadGraphs() {
        if ( !chart)
            return

        var currxaxis = xas;
        var xaxistype = chart.xaxisType;
        console.log("type=" + xaxistype)
        if (xaxistype == 3) {   // 3 == chart.AxisType.AT_CATEGORIES
            var ser = chart.getSeries(0)
            var xcat = ser.categories("xaxis");
            var keys = xcat.keys;
            var labels = xcat.labels;
            currxaxis = [];
            itemaxisx.min = ser.minID;
            itemaxisx.max = ser.maxID;
            for (var i = 0; i < keys.length; i++) {
                itemaxisx.append(labels[i], keys[i])
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
			    series.color = chart.seriesColor(i);
			    var points = smodel.points;
			    var bar = series.append(smodel.name, points);
                bar.color = chart.seriesColor(i);
            }
            if (ctype == "pie") {
			    var points = smodel.points;
                var slice = series.append("pipo", 10)
                var slice = series.append("mameloe", 15)
                var slice = series.append("dikkedeur", 7)

                for (var j = 0; j < points.length; j++) {
                console.log("pnt: " + j+1 + " = " + points[j])
//                    var slice = series.append(smodel.name + (j+1), points[j].y)
                }
            }
		}
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
}