import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Item {
    //width: parent ? parent.width : 0
    height: parent ? parent.height - 10 : 0
    property var editor
    x : 5

    Row {
        width : parent.width
        height : 20
        spacing : 4
        Text {
            width : Math.min(parent.width*  0.30, 60)
            height : 20
            text : qsTr("Cell size")
            elide : Text.ElideRight
            anchors.verticalCenter : parent.verticalCenter
        }
        TextField{
            id : distanceValue
            width : Math.min(60,parent.width * 0.20)
            height : 20
            text : editor.distance
        }

        Button{
            height : 18
            width :  Math.min(parent.width * 0.55, 60)
            text : qsTr("Apply")
            onClicked: {
                editor.distance = parseFloat(distanceValue.text)
            }
        }
    }
}

