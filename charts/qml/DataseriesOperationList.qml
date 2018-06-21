import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../.." as Base
import ChartModel 1.0
import DataseriesModel 1.0

Item {
    width: 140
    x : 5
    y : 5
    height: parent.height -10
    property var currentSeries          // dataseries model

     function setOperations(operations) {
        operationlist.model = operations
     }

    function setOperation(newindex){
        operationlist.currentIndex = newindex
        dataseriesoperation.seriesIndex = newindex
        if ( operationlist.model && newindex < operationlist.model.length ){
            dataseriesoperation.currentOperation = operationlist.model[newindex]
        }
    }

    onCurrentSeriesChanged: {
        if ( currentSeries)
            operationlist.model = currentSeries.operations
    }

    Rectangle {
        id : title
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Operations")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : title
            text : qsTr("All operations that are applicable to the dataseries")
        }
    }

    ListView {
        id : operationlist
        width : parent.width - 3
        height : parent.height - title.height - 5
        anchors.top : title.bottom
        anchors.topMargin: 4
        x : 3
        Component {
            id: operationHighlight

            Rectangle {
                width: operationlist.width - 6; height: 14
                x : 3
                color: Global.selectedColor; radius: 2
                y: (operationlist && operationlist.currentItem) ? operationlist.currentItem.y : 0
            }
        }
        highlight: operationHighlight
        delegate :  Component {
            Loader {
                sourceComponent: Component {
                    Text {
                        x : 4
                        text: name
                        width : operationlist.width
                        height : 14
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                setOperation(index)
                            }
                        }
                    }
                }
            }
        }

    }
}

