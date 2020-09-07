import QtQuick                      2.7
import QtQuick.Controls.Material    2.1

import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan

Qan.EdgeItem {
    id: edgeItem
    property color  color: Qt.rgba(1,0,0,1)
	property color lineColor : uicontext.darkColor
    Qan.EdgeTemplate {
        anchors.fill: parent
        edgeItem: parent
        color: lineColor
    }
}
