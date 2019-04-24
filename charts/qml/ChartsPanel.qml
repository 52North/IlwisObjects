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
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls

Item {
    id : chartspanel
    anchors.fill : parent
    anchors.topMargin: 4
    objectName:  uicontext.uniqueName()
    property ChartModel chart
    property string iconName : "../images/graph"
    property TabModel tabmodel
    property TableModel tabledata
	property var showManager : true
	property var updateDataTable : chart ? chart.updateDataTable : false

	onUpdateDataTableChanged : {
		if ( datatab.item){
			datatab.item.setTable(chart.tableModel)
		}
	}

	signal click(int mx,int my)

	ChartToolBar {
		id : toolbar1
		visible : showManager
	}

	Rectangle {
	    id : zoomRectangle
	    x : 0
	    y : 0
	    width : 0
	    height : 0
	    visible : false
	    border.width : 1
	    border.color : "#e6f9ff"
	    color : "transparent"
	    z : 100

		function disable() {
			x = 0
		    y = 0
		    width = 0
		    height = 0
		    visible = false
			activeToolBar().zoomMode = false
		}

	}

	Rectangle {
		anchors.fill : zoomRectangle
		color : "grey"
		opacity : 0.2
		z: 101
	}

    TabView {
        id : chartarea
        anchors.top : toolbar1.bottom
		anchors.bottom : parent.bottom
		width : parent.width
        Tab {
            id : charttab
            title: "Chart"
            active : true
            SplitView {
                anchors.fill: parent
                orientation: Qt.Vertical
                height : parent.height
				function zoomReset(){
					chartpane.zoomReset()
				}
                ChartPane {
				  Connections {
						target: chartpane
						onClick: {
							click(mx,my)
						 }
					}

                    id : chartpane
                    //height : parent.height - propertiespanel.height
                }

                ChartManagement {
                    id : propertiespanel
                    height : showManager ? Global.actionBarMaxHeight : 0
                    width : parent.width
					visible : showManager
                }
            }
        }
        Tab {
            id : datatab
            title : "Data"
            active : true

            TablePanel.TablePane {
                id : chartTable
            }
        }
    }

    function addDataSource(filter, sourceUrl, sourceType){
		var parts = filter.split("=");
		chart = models.model(parts[1]);
        chart.assignParent(chartspanel);
		tabmodel.displayName = chart.name

		setDataTabTableData()
    }

	function setDataTabTableData(){
		var filter = "itemid=" + chart.dataTableId()
        datatab.item.addDataSource(filter, chart.dataTableUrl(), "table")
		chart.tableModel = datatab.item.table
	}

	function activeToolBar() {
		return toolbar1
	}

	function zoomReset(){
		chartarea.getTab(0).item.zoomReset()
	}

	Component.onDestruction :{
		models.unRegisterModel(chart.modelId())
	}

}

