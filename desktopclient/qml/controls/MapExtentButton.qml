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

MouseArea {
    hoverEnabled: true
    height : 25
    width : 25
    property string icon
    property bool pushed
    property bool hovering: containsMouse
    property real displacement: 2.0
    property alias action: button.action
    property alias checked: button.checked
    property alias checkable: button.checkable
    property alias pressed: button.pressed
    property alias enabled: button.enabled
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../images/" + name
        return iconP
    }

    Button {
        id: button
        anchors.fill: parent

        Component.onCompleted: {
            __behavior.propagateComposedEvents = true // Workaround: the button's own MouseArea that is inside BasicButton.qml doesn't properly propagate the 'hovered' property, so we need to create our own MouseArea and compute it ourselves.
        }

        Rectangle {
            id: buttonrect
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
            anchors.fill: buttonrect
            visible : enabled && hovering && !pushed
            horizontalOffset: displacement
            verticalOffset: displacement
            radius: 1.0
            samples: 3
            color: "#80000000"
            source: buttonrect
        }

        InnerShadow {
            anchors.fill: buttonrect
            visible: pushed
            horizontalOffset: displacement
            verticalOffset: displacement
            radius: 1.0
            samples: 3
            fast: true
            color: "#b0000000"
            source: buttonrect
        }
    }
}
