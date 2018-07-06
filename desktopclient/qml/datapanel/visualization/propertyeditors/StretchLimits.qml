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
        width : parent.width - 60
        height: parent ? parent.height - 10 : 0
        x : 10

        //spacing : 5

        Loader {
            id : chartArea
            width : parent.width
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
        Connections {
            target: slider
            onMarkerPositions :{
                editor.setMarkers(positions)
            }  
        }
            
        Controls.MultiPointSlider {
            id : slider
            model : editor.markers
            minValue : editor.min
            maxValue : editor.max
            width : parent.width - 20
            resolution : editor.resolution
            x : 12
        }
    }
    Column {
        width : 50
        height :110

        Text {
            width : parent.width
            height : 24
            text : "Presets"
        }
        Button {
            width : parent.width
            height : 20
            text : "0%"
            onClicked : {
                updateMarkerPositions(0)
            }
        }

        Button {
            width : parent.width
            height : 20
            text : "1%"

            onClicked : {
                var limits = []
                updateMarkerPositions(0.01)
            }
        }

        Button {
            width : parent.width
            height : 20
            text : "2%"

            onClicked : {
                updateMarkerPositions(0.02)
            }
        }

        Button {
            width : parent.width
            height : 20
            text : "5%"

            onClicked : {
                updateMarkerPositions(0.05)
            }
        }
    }
    function updateMarkerPositions(fraction){
        editor.setStretchLimit(fraction)
        slider.paint()
    }
}

