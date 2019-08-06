import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.3
import "../Global.js" as Global
import "../controls" as Controls

Controls.PushButton{
    property string iconsource
    property string buttontext;
    property string backColor : "white"
	clip : true

    id : addContainer
    Image {
        id : buttonIcon
        anchors.horizontalCenter: parent.horizontalCenter
        source: iconsource
    }
    Text {
        anchors.bottom: addContainer.bottom
        anchors.bottomMargin: 2
        anchors.horizontalCenter: parent.horizontalCenter
        text : buttontext
        font.pointSize: 9
    }

    /*style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? uicontext.paleColor : backColor
        }
    }*/

    onClicked: {
        if ( typeof addContainer.mouseClicked === "function"){
            checked = !checked
            mouseClicked()
        }
    }

}
