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

    Text {
        id : colorselectorlabel
        text : "Color"
    }
    Controls.ColorPicker2 {
        id : chartcolorselector
        initialColor : dataseriesOperationList.currentSeries ? dataseriesOperationList.currentSeries.color : "black"

        anchors.top : colorselectorlabel.bottom
        onSelectedColorChanged: {
            var oldIndex =  dataserieslist.currentIndex
            var paramaters = {seriesname:dataseriesOperationList.currentSeries.name, color : chartcolorselector.selectedColor}
            operation.execute(paramaters)
            dataserieslist.currentIndex = oldIndex
        }

    }

}

