import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "." as Charts
import ChartModel 1.0
import TableModel 1.0
import TabModel 1.0
import "../../../../qml" as Base

Item {
    id : chartspanel
    anchors.fill : parent
    anchors.topMargin: 4
    objectName:  uicontext.uniqueName()
    property ChartModel chart
    property string iconName : "../images/graph"
    property TabModel tabmodel

    TabView {
        id : chartarea
        anchors.fill : parent
        Tab{
            id : charttab
            title: "Chart"
            active : true
            SplitView {
                anchors.fill: parent
                orientation: Qt.Vertical
                height : parent.height
                ChartPane {
                    id : chartpane
                    height : parent.height - 270
                }

                ChartProperties {
                    id : propertiespanel
                    height : 270
                    width : parent.height
                }
            }
        }
        Tab {
            id : datatab
            title : "Data"
        }
    }

    function addDataSource(filter, sourceUrl, sourceType){
		var parts = filter.split("=");
		chart = models.model(parts[1]);
        chart.parent = chartspanel;
		tabmodel.displayName = chart.name
		chartpane.chart = chart
    }

}

