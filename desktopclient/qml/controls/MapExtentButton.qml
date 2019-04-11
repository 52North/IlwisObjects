import QtQuick 2.2
import QtGraphicalEffects 1.12
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
//import "./propertyeditors" as LayerManagement
import "../controls" as Controls
import "../Global.js" as Global

Button {
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../images/" + name
        return iconP
    }

    property string icon
    property bool pushed: true // not to be confused with the built-in Button property "pressed"; default "pushed = true" to get the old behavior (a flat button) if we forget to set this property
    property int displacement: 2.0
    height : 25
    width : 25
    Rectangle {
        id: button
        anchors.fill: parent
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenterOffset: pushed ? displacement / 2.0 : 0
            anchors.horizontalCenterOffset: pushed ? displacement / 2.0 : 0
            source : iconsource(icon)
        }
    }

     DropShadow {
        anchors.fill: button
        visible : !pushed
        horizontalOffset: displacement
        verticalOffset: displacement
        radius: 1.0
        samples: 3
        color: "#80000000"
        source: button
    }
}

