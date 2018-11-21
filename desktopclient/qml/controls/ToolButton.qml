import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../Global.js" as Global

Button {
    id : but
    height : 25
    width : 25
    property alias iconSource : image.source
    property alias tooltip : ttip.text
    property var type
    property var callBack
    property var iconH : 20
    property var iconW : 20
    property var background : Global.selectedColor

    Rectangle {
        anchors.centerIn: parent
        height : iconH + 4
        width : iconW + 4
        color : but.checked ? background : "transparent"


        Image {
            x : 2
            y : 2
            id : image
            height : iconH
            width : iconW
        }

    }
    onClicked: {
        if ( callBack){
           callBack(type)
           
        }
    }

    ToolTip{
        id : ttip
        target : parent
    }
}
