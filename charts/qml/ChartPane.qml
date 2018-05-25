import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.5
import QtCharts 2.0
import ChartModel 1.0
import DataseriesModel 1.0

Rectangle {
    id : chartView
    width : parent.width
    height : parent.height - 270
    color : "lightgrey"
	property ChartModel chart
    property var updateChart : chart ? chart.updateSeries : 0

    onUpdateChartChanged : {
        visibleGraphs.removeAllSeries()
        loadGraphs()    
    }
	function loadGraphs() {
        if ( !chart)
            return
		for (var i = 0; i < chart.seriesCount; i++) {
			var smodel = chart.getSeries(i);
            var series = createSeries(chart.chartType, smodel.name, xas, yas)
			series.pointsVisible = false;
			series.color = Qt.rgba(Math.random(),Math.random(),Math.random(),1);
//			series.hovered.connect(function(point, state){ console.log(point); }); // connect onHovered signal to a function
			var points = smodel.points
			var pointsCount = points.length;
			for (var j = 0; j < pointsCount; j++) {
				series.append(points[j].x, points[j].y);
			}
		}
	}

    function createSeries(ctype, name, xas, yas){
        if ( ctype == "line"){
            return visibleGraphs.createSeries(ChartView.SeriesTypeLine, name, xas, yas);
        }
        if (ctype == "spline"){
            return visibleGraphs.createSeries(ChartView.SeriesTypeSpline, name, xas, yas);
        }
        if (ctype == "bar")
            return visibleGraphs.createSeries(ChartView.SeriesTypeBar, name, xas, yas);
        if ( ctype == "pie")
            return visibleGraphs.createSeries(ChartView.SeriesTypePie, name, xas, yas);
        if ( ctype == "points")
            return visibleGraphs.createSeries(ChartView.SeriesTypeScatter, name, xas, yas);
        if ( ctype == "polar")
            return visibleGraphs.createSeries(ChartView.SeriesTypeScatter, name, xas, yas);
        if ( ctype == "area")
            return visibleGraphs.createSeries(ChartView.SeriesTypeArea, name, xas, yas);
        return visibleGraphs.createSeries(ChartView.SeriesTypeLine, name, xas, yas);
    }

	onChartChanged : {
		loadGraphs();
	}

	ValueAxis {
		id : xas
		min : chart != null ? chart.minX : 0
		max : chart != null  ? chart.maxX : 5
		tickCount : chart ? chart.tickCountX : 5
	}

	ValueAxis {
		id : yas
		min : chart != null  ? chart.minY : 0
		max : chart != null  ? chart.maxY : 5
		tickCount : chart ? chart.tickCountY : 5
	}

    Rectangle {
    anchors.fill: parent
    color : "red"
	ChartView {
		id : visibleGraphs
		title: "Line"
		anchors.fill: parent
		antialiasing: true
        theme : ChartView.ChartThemeBlueIcy
	}
    }
}