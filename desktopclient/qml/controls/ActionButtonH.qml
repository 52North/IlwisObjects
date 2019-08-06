import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.3
import "../Global.js" as Global
import "../controls" as Controls

Controls.PushButton{
    property string iconsource
    property string buttontext;

    id : addContainer
    Image {
        id : buttonIcon
        anchors.verticalCenter: parent.verticalCenter
        source: iconsource
        x : 5 ;width : 25; height : 25
        fillMode: Image.PreserveAspectFit
    }
    Text {
        anchors.left: buttonIcon.right
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        text : buttontext
    }
}


