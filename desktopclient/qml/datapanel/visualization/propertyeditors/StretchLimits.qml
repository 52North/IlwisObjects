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
        id : minihist
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

console.log("zoom? ", editor.zoomOnPreset);
console.log("str_max: ", editor.max);
console.log("chart_max: ", chartArea.item.xmax);
console.log("slid_max: ", slider.maxValue);
                 if (editor.zoomOnPreset) {
                    chartArea.item.xmin = editor.min;
                    chartArea.item.xmax = editor.max;
                    slider.minValue = editor.min;
                    slider.maxValue = editor.max;
console.log("met zoom")
console.log("str_max: ", editor.max);
console.log("chart_max: ", chartArea.item.xmax);
console.log("slid_max: ", slider.maxValue);
                }
            }
        }
        Connections {
            target: slider
            onMarkerPositions :{
                editor.setMarkers(positions)
            }  
        }
        Connections {
            target: slider
            onMarkerReleased :{
                editor.markersConfirmed()
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
        width : 70
        height :110
        anchors.leftMargin : 5
        anchors.rightMargin : 5

        Text {
            width : parent.width
            height : 24
            text : "Presets"
        }
        Button {
            width : parent.width - 20
            height : 20
            text : "0%"
            onClicked : {
                updateMarkerPositions(0)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "1%"

            onClicked : {
                var limits = []
                updateMarkerPositions(0.01)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "2%"

            onClicked : {
                updateMarkerPositions(0.02)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "5%"

            onClicked : {
                updateMarkerPositions(0.05)
            }
        }
        Text {
            width : parent.width
            height : 24
            text : "Zoom on preset"
        }
        Switch {
            id : zoomonpreset
            checked : editor.zoomOnPreset

            onClicked : {
                editor.zoomOnPreset = checked

                if (!zoomonpreset.checked) {
                    updateMarkerPositions(editor.zoomLevel);
                    slider.minValue = editor.min;
                    slider.maxValue = editor.max;
                    slider.paint();
                }

            }

        }
    }
    function updateMarkerPositions(fraction){
        editor.setStretchLimit(fraction); // calls editor.setMarkers(positions)
        editor.markersConfirmed();

        if (zoomonpreset.checked) {
            chartArea.item.xmin = editor.min;
            chartArea.item.xmax = editor.max;
            slider.minValue = editor.min;
            slider.maxValue = editor.max;
        }

        slider.paint();
    }
}

