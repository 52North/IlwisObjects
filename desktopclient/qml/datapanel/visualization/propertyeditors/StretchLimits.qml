import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5 as QC2
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Row {
    width : Math.min(500,parent ? Math.min(parent.width,500) : 500)
    height: parent ? parent.height - 10 : 0
    clip : true
    property var editor
    property int offset : 30
	property bool updateMarkers : false
    Column {
        id : minihist
        width : parent.width - 60
        height: parent ? parent.height - 10 : 0
        x : 10

        Loader {
            id : chartArea
            width : parent.width + offset
            height : parent.height - control.height - 10
            source : models.mainPanelUrl("minimalchart")
            x : -offset

            onLoaded : {
			    var extraParams = {name : 'histogram',chartType : 'line', color : 'black', attribute : editor.visualAttribute.attributename}
                chartArea.item.chart.addDataTable(editor.coverageId,"max","histogram",extraParams)
                chartArea.item.margins.left = 0
                chartArea.item.margins.right = 0
                chartArea.item.margins.top = 0
                chartArea.item.margins.bottom = 0
				item.height = parent.height
            }
        }
                
		QC2.RangeSlider {
			id : control
			x: 12
			width : parent.width - 20
			from : editor.min
			to : editor.max

			first.value: editor.initStretchMin
			second.value: editor.initStretchMax

			first.handle: Rectangle {
				x: control.leftPadding + control.first.visualPosition * (control.availableWidth - width)
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 26
				implicitHeight: 26
				radius: 13
				color: control.first.pressed ? "#f0f0f0" : "#f6f6f6"
				border.color: "#bdbebf"

				Text {
					anchors.centerIn : parent
					text : control.first.value.toFixed(2)
					font.pointSize : 6
				}

			}

			first.onPressedChanged : {
				if ( !control.first.pressed){
					editor.markersConfirmed()

				}
			}

			first.onValueChanged : {
				var positions = []
				positions.push(control.first.value)
				positions.push(control.second.value)
				if ( updateMarkers)
					editor.setMarkers(positions)
			}

			second.onPressedChanged : {
				if ( !control.second.pressed){
					editor.markersConfirmed()

				}
				
			}
			second.onValueChanged : {
				var positions = []
				positions.push(control.first.value)
				positions.push(control.second.value)
				if ( updateMarkers)
					editor.setMarkers(positions)
			}

			second.handle: Rectangle {
				x: control.leftPadding+ control.second.visualPosition * (control.availableWidth - width)
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 26
				implicitHeight: 26
				radius: 13
				color: control.second.pressed ? "#f0f0f0" : "#f6f6f6"
				border.color: "#bdbebf"

				Text {
					anchors.centerIn : parent
					text : control.second.value.toFixed(2)
					font.pointSize : 6
				}
			}
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
    }
    function updateMarkerPositions(fraction){
        editor.setStretchLimit(editor.visualAttribute.attributename, fraction); // calls editor.setMarkers(positions)
		updateMarkers = false
		control.first.value = fraction !== 0 ? editor.initStretchMin : editor.min;
		control.second.value = fraction !== 0 ? editor.initStretchMax : editor.max;
		updateMarkers = true
        editor.markersConfirmed();
    }
}

