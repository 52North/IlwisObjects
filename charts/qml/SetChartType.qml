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
    width: parent.width - 5
    height: parent ? parent.height - 10 : 0
    property var operation

    Controls.ComboxLabelPair {
        id : charttypeselector
        labelText : qsTr("Chart type")
        labelWidth : 100
        width : parent.width - 5
        height : 20
        itemModel : ["Line", "Spline", "Bar", "Pie", "Points" ]//, "Polar", "3DLine", "3DSpline", "3DBar" ]

        Component.onCompleted : {
            charttypeselector.initialComboText = dataseriesOperationList.currentSeries.charttype
        }

        onIndexChanged : {
            var paramaters = {seriesname:dataseriesOperationList.currentSeries.name, charttype : charttypeselector.comboText}
            operation.execute(paramaters)
        }
    }

}

