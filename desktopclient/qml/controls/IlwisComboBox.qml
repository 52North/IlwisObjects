import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1


Rectangle {
    id: contextPanel
    property int panelHeight
    property color backgroundColor : "transparent"
    property var expandableArea : internealArea
    property string buttonIcon : "../images/arrowdown.png"
    property alias labelText: titleTxt.labelText
	property alias labelWidth :  titleTxt.labelWidth
    property int editheight : 20
	property alias checkFunction : titleTxt.checkFunction
	property alias callbackFunction : titleTxt.callbackFunction
	property alias content : titleTxt.content

    Rectangle {
        id: titleRect
        width: parent.width
        height: editheight
        color : backgroundColor

        TextEditLabelPair{
            id: titleTxt
            width : parent.width - titleArrow.width
            anchors.verticalCenter: parent.verticalCenter
        }

        Image {
            id: titleArrow
            width:  14
            height:  14
            anchors.right : parent.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: buttonIcon
			fillMode : Image.PreserveAspectFit 

        }
        MouseArea{
            width: parent.width
            height:  parent.height
            onClicked: {
                if (contextPanel.state == "collapsed")
                {
                    contextPanel.state = "open"
                }
                else
                {
                    contextPanel.state = "collapsed"
                }
            }
        }
    }

    Rectangle {
        id : internealArea
        width: parent.width
        anchors.top: titleRect.bottom
        opacity : 0
        color : backgroundColor
        clip : true
    }

    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: internealArea
                height: 0
                opacity: 0
            }
            PropertyChanges {
                target: titleArrow
                rotation: 0
            }
        },
        State{
            name: "open"
            PropertyChanges {
                target: internealArea
                height: panelHeight
                opacity: 1
            }
            PropertyChanges {
                target: titleArrow
                rotation: 90
            }
            PropertyChanges {
                target: contextPanel
                height : panelHeight + titleRect.height
            }
        }

    ]

    transitions: [
        Transition {
            NumberAnimation { target: contextPanel; property: "height"; duration: 300 }
            NumberAnimation { target: internealArea; property: "height"; duration: 300 }
            NumberAnimation { target: internealArea; property: "opacity"; duration: 300 }
            NumberAnimation { target: titleArrow; property: "rotation"; duration: 300 }
        }
    ]

    Component.onCompleted:  {
        contextPanel.height = titleRect.height+ ( state == "open"? panelHeight : 0)
    }
}
