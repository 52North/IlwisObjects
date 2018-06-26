import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../.." as Base
import ChartModel 1.0
import DataseriesModel 1.0

Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation

/*    Text {
        id : colorselectorlabel
        text : "Chart type"
    }*/

    Controls.ComboxLabelPair {
        id : charttypeselector
        labelText : qsTr("Chart type")
        labelWidth : 100
        width : parent.width - 5
        height : 20
        itemModel : ["Line", "Spline", "Bar", "Pie", "Points", "Polar", "3DLine", "3DSpline", "3DBar" ]

        onIndexChanged : {
//            var oldIndex =  dataserieslist.currentIndex
            var paramaters = {seriesname:dataseriesOperationList.currentSeries.name, charttype : charttypeselector.comboText}
            operation.execute(paramaters)
//            dataserieslist.setCurrentIndex(oldIndex)
        }
    }

}

