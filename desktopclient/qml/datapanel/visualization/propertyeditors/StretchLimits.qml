import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Row {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height: parent ? parent.height - 10 : 0
    property var editor
    Column {
        width : parent.width - 50
        height: parent ? parent.height - 10 : 0

        spacing : 5

        Loader {
            id : chartArea
            width : slider.width
            height : parent.height - slider.height - 10
            source : models.mainPanelUrl("minimalchart")

            onLoaded : {
                chartArea.item.chart.addDataTable(editor.coverageId,"max","counts","", "black")
                chartArea.item.margins.left = 0
                chartArea.item.margins.right = 0
                chartArea.item.margins.top = 0
                chartArea.item.margins.bottom = 0
            }
        }
        Controls.MultiPointSlider {
            id : slider
            model : editor.markers
            minValue : editor.min
            maxValue : editor.max
            width : parent.width
        }
    }
    Rectangle {
        width : 30
        height : chartArea.height
        border.width : 1
    }
}

