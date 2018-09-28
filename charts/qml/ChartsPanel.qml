import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "." as Charts
import ChartModel 1.0
import TabModel 1.0
import TableModel 1.0
import "../../../../qml" as Base
import "../../../../qml/datapanel/table" as TablePanel

Item {
    id : chartspanel
    anchors.fill : parent
    anchors.topMargin: 4
    objectName:  uicontext.uniqueName()
    property ChartModel chart
    property string iconName : "../images/graph"
    property TabModel tabmodel
    property TableModel tabledata

    TabView {
        id : chartarea
        anchors.fill : parent
        Tab {
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

                ChartManagement {
                    id : propertiespanel
                    height : 270
                    width : parent.height
                }
            }
        }
        Tab {
            id : datatab
            title : "Data"

            TablePanel.TablePane {
                id : chartTable

                Component.onCompleted : {
                    chartTable.addDataSource("", chart.dataTableUrl(), "table")
                }
            }
        }
    }

    function addDataSource(filter, sourceUrl, sourceType){
		var parts = filter.split("=");
		chart = models.model(parts[1]);
        chart.assignParent(chartspanel);
		tabmodel.displayName = chart.name

        tabledata = models.createTableModel(chartspanel, chart.dataTableUrl(), "table")
        console.log("datatable="+tabledata)
//        chartTable.addDataSource(filter, chart.dataTableUrl(), "table")
//        chartspanel.chartarea.datatab.chartTable.table = tabledata
    }

	Component.onDestruction :{
	    console.debug("closing chart panel")
		models.unRegisterModel(chart.modelId())
	}

}

