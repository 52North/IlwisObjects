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

    function setCurrentIndex(newindex){
        if (chart) {
            dataserieslist.currentIndex = newindex
            dataseriesOperationList.currentSeries = dataserieslist.model[newindex]

            dataseriesOperationList.setOperation(0)
        }
    }

    Rectangle {
        id : title
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Dataseries")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : title
            text : qsTr("All dataseries in the chart")
        }
    }

    Rectangle {
        id : chartlabel
        width : parent.width - 2
        height : 18
        anchors.top : title.bottom
        clip : true
        color : uicontext.paleColor
        Text {
            text : qsTr("Chart")
            width : parent.width
//            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : chartlabel
            text : qsTr("Global properties of the chart")
        }
    }

    ListView {
        id : dataserieslist
        width : parent.width - 3
        height :    parent.height - title.height
        anchors.top : chartlabel.bottom
        model : chartspanel.chart ? chartspanel.chart.series : null
        onModelChanged: {
            setCurrentIndex(0)
        }

        anchors.topMargin: 4
        x : 3
        clip : true
        highlight: attributeHighlight
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
/*                spacing : 3
                Image{
                    id : domicon
                    source : dataserieslist.iconsource(icon)
                    width : 16
                    height : 16
                    anchors.verticalCenter: parent.verticalCenter
                }*/
                Text {
                    text :  modelData.name
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            setCurrentIndex(index)
                        }
                    }
                }
            }
        }
    }
}

