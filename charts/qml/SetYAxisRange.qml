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

    Controls.TextEditLabelPair{
        id : yaxisrangelow
        width : 300
        height : 20
        labelText: qsTr("Y-axis range low")
        labelWidth: 140
        regexvalidator: /^\d*(\.\d*)?$/

        Component.onCompleted : {
            yaxisrangelow.content = chart.minY
        }
    }

    Controls.TextEditLabelPair{
        id : yaxisrangehigh
        width : 300
        height : 20

        anchors.top : yaxisrangelow.bottom

        labelText: qsTr("Y-axis range high")
        labelWidth: 140
        regexvalidator: /^\d*(\.\d*)?$/

        Component.onCompleted : {
            yaxisrangehigh.content = chart.maxY
        }
    }

    Row {
        id : checkfixed
        anchors.top : yaxisrangehigh.bottom

        CheckBox {
            id : fixedYaxis

            width : 20
            height : 20
            Component.onCompleted : {
                checked = chart.fixedY
            }
        }
        Text {
            text : qsTr("Lock Y-axis range")
            height : 20
            width : 250
        }
    }

    Button {
        width : 80
        height : 20

        anchors.top : checkfixed.bottom
        anchors.right: yaxisrangelow.right
        anchors.rightMargin: 3

        text : qsTr("Apply")
        onClicked: {
            var newlow = chart.minY
            var newhigh = chart.maxY
            if ( yaxisrangelow.content != "") {
                newlow  = parseFloat(yaxisrangelow.content)
            }
            if ( yaxisrangehigh.content != "") {
                newhigh  = parseFloat(yaxisrangehigh.content)
            }
            var parameters = {low:newlow, high:newhigh, fixed : fixedYaxis.checked}
            operation.execute(parameters)
        }
    }

}

