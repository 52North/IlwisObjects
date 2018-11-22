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


Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation

    function applyChanges() {
        if (chartlegendalignment.comboText == "")
            return

        var parameters = {show:checkshowlegend.checked, align:chartlegendalignment.comboText}
        operation.execute(parameters)
    }

    Row {
        id : checkvisible
        x : 10

        CheckBox {
            id : checkshowlegend

            width : 20
            height : 20
            Component.onCompleted : {
                checked = chart.legendVisible
            }
            onClicked : {
                applyChanges()
            }
        }
        Text {
            text : qsTr("Show Legend")
            height : 20
            width : 250
        }
    }

    Controls.ComboxLabelPair {
        id : chartlegendalignment
        labelText : qsTr("Legend position")
        labelWidth : 100
        width : parent.width - 5
        height : 20
        anchors.top : checkvisible.bottom

        itemModel : ["Top", "Right", "Bottom", "Left" ]

        Component.onCompleted : {
            chartlegendalignment.initialComboText = chart.legendAlignment
        }

        onComboTextChanged : {
            applyChanges()
        }

    }
}

