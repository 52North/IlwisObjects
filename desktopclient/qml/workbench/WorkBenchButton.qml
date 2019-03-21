import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../controls" as Controls
import "../Global.js" as Global

ToolButton{
    property string iconname
    id : button
    property string label
    width : parent.width
	property var bgcolor :  uicontext.middleColor

    checkable: true
    checked: false
    onWidthChanged: {
        if ( width > 50){
            textLabel.width = button.width
            textLabel.visible = true
        }else
            textLabel.visible = false
    }

    onClicked: {
        if ( typeof button.mouseClicked === "function"){
            checked = !checked
            mouseClicked()
        }
    }

    Image { id : but
        anchors.centerIn: parent
        source: iconname == "" ? "" : "../images/" + iconname
        fillMode: Image.PreserveAspectFit
        width : 40
        height: width
    }
    Text {
        id : textLabel
        anchors.top : but.bottom
        width : 0
        height : 15
        text : label
        horizontalAlignment: Text.AlignHCenter
        visible : true
        font.weight: Font.DemiBold
        font.pixelSize: 12
        color : "white"
        opacity : button.height > 70 ? 1 : 0
   }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : bgcolor
        }
    }
    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: button
                height : butColumn.butheight
                opacity : 1
                enabled : true
            }
        },
        State {
            name : "zerosize"
            PropertyChanges {
                target: button
                height : 0
                opacity : 0
                enabled : false
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 300 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 300 ; easing.type: Easing.InOutCubic }
        }
    ]

    Component.onCompleted: {
        button.state = "fullsize"
    }

}
