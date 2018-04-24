import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "./charts" as Charts
import ChartModel 1.0
import TableModel 1.0
import TabModel 1.0

Item {
    id : chartspane
    anchors.fill : parent
    anchors.topMargin: 4
    objectName:  uicontext.uniqueName()
    property ChartModel chart
    property string iconName : "../images/graph"
    property TableModel table
    property TabModel tabmodel
    property ChartModel chart


    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        height : parent.height
        ChartPane{
            id : chartpanel
            height : parent.height - 270
        }

        ChartProperties {
            id : propertiespanel
            height : 270
            width : parent.height
        }
    }

    function addDataSource(sourceUrl, filter, sourceType){
       chart = uicontext.createChartModel(chartspane,sourceUrl, sourceType)
    }

}

