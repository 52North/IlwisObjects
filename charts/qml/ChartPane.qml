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

	function loadGraphs() {
		for (var i = 0; i < chart.seriesCount; i++) {
			var smodel = chart.getSeries(i);
			var series = visibleGraphs.createSeries(chart.chartType, smodel.name, xas, yas);
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

	onChartChanged : {
		loadGraphs();
	}

	ValueAxis {
		id : xas
		min : chart != null ? chart.minX : 0
		max : chart != null  ? chart.maxX : 5
		tickCount : 5
	}

	ValueAxis {
		id : yas
		min : chart != null  ? chart.minY : 0
		max : chart != null  ? chart.maxY : 5
		tickCount : 5
	}

	ChartView {
		id : visibleGraphs
		title: "Line"
		anchors.fill: parent
		antialiasing: true

/*		Component.onCompleted: {
		console.log("Attach the data series")
		console.log(chartspane)
		console.log(chartspane.chart)
//		console.log(chartspane.chart.seriesCount);

//			visibleGraphs.addSeries(myline)
			var line1 = visibleGraphs.createSeries(ChartView.SeriesTypeLine, "Manual created", xas, yas)
			line1.append(0,0);
			line1.append(1.1,2.1);
			line1.append(1.9,3.3);
			line1.append(2.1,2.1);
			line1.append(2.9,4.9);
			line1.append(3.4,3.0);
			line1.append(4.1,3.3);
		}*/


	}
}