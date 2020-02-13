import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../.." as Base
import ChartModel 1.0
import DataseriesModel 1.0

Item {
    id : inner
    width: 140
    x : 5
    y : 5
    height: parent.height -10
    property alias currentIndex : dataserieslist.currentIndex
	property var oldIndex : -1

    Rectangle {
        id : title
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Data series")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : title
            text : qsTr("All data series in the chart")
        }
    }

    ListView {
        id : chartlabel
        width : parent.width - 2
        height : 18
        anchors.top : title.bottom
        model : ["Chart"]
        highlight: charthighlight
        currentIndex : -1

        anchors.topMargin: 4
        x : 3
        clip : true

        Controls.ToolTip {
            target : chartlabel
            text : qsTr("Global properties of the chart")
        }

        delegate : Component {
            Row {
                width : 130
                height : 18
                Text {
                    text :  modelData
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            setChartIndex()
                        }
                    }
                }
            }
        }

        Component {
            id: charthighlight

            Rectangle {
                width: chartlabel.width - 6; height: 14
                color: Global.selectedColor; radius: 2
                y: (chartlabel && chartlabel.currentItem) ? chartlabel.currentItem.y : 0
            }
        }
    }

    ListView {
        id : dataserieslist
        width : parent.width - 3
        height :    parent.height - title.height
        anchors.top : chartlabel.bottom
        model : chartspanel.chart ? chartspanel.chart.series : null
        anchors.topMargin: 4
        x : 3
        clip : true
        highlight: attributeHighlight

        onModelChanged: {
            if (model) {
                setChartIndex()
				setOperIndex(oldIndex)
				//chartlabel.currentIndex = -1
				//dataserieslist.currentIndex = oldIndex	
				//dataseriesOperationList.setOperation(-1)
            }
        }

        function iconsource(name) {
            if ( name.indexOf("/") !== -1)
                return name
            if ( name === "")
                name = "redbuttonr.png"

            var iconP = "../../images/" + name
            return iconP
        }

        Component {
            id: attributeHighlight

            Rectangle {
                width: dataserieslist.width - 6; height: 14
                color: Global.selectedColor; radius: 2
                y: (dataserieslist && dataserieslist.currentItem) ? dataserieslist.currentItem.y : 0
            }
        }
        delegate : Component {
            Row {
                width : 130
                height : 18
                Text {
                    text :  modelData.name
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            setOperIndex(index)
                        }
                    }
                }
            }
        }
    }

    function setOperIndex(newindex) {
        chartlabel.currentIndex = -1
        dataserieslist.currentIndex = newindex
        if (chart) {
            dataseriesOperationList.currentSeries = dataserieslist.model[newindex]
            dataseriesOperationList.setOperation(-1)
			oldIndex = newindex
        }
    }

    function setChartIndex() {
        if (chart) {
            dataseriesOperationList.currentSeries = chart
        }
        dataserieslist.currentIndex = -1
        chartlabel.currentIndex = 0
    }

}

