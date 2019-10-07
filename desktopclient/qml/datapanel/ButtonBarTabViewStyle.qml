import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../Global.js" as Global
import "../controls" as Controls

TabViewStyle {
    property string selectColor : uicontext.actionHeaderColor
    property string nonselectColor : uicontext.actionColor
    property int tabHeight : 30


    frameOverlap: 1
    tab: Rectangle {
        id : labelArea
        color : styleData.selected ? selectColor :  nonselectColor
        border.color:  selectColor

        implicitWidth: control.width / control.count
        implicitHeight: tabHeight
		clip : true

        Controls.ToolTip{
            id : ttip
            target : labelArea
            up : true
        }

       /* Image {
            source : control.getTab(styleData.index).item ? (styleData.selected ? control.getTab(styleData.index).item.iconName + "_green.svg" : control.getTab(styleData.index).item.iconName + "_grey.svg") : ""
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
        }*/

        Text {
            id: text
			anchors.verticalCenter : parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment:Text.AlignHCenter

            text: styleData.title
            color: styleData.selected ? uicontext.darkestColor : "grey"
            font.pointSize: 9
            font.weight: Font.DemiBold
        }

        MouseArea  {
            anchors.fill : parent
            onClicked: {
                control.tabClicked(styleData.index)
            }
        }

        Component.onCompleted: {
            if ( 'tooltips' in control ){
                ttip.text = control.tooltips[styleData.index]
            }
        }
    }
    frame: Rectangle { color: uicontext.lightestColor; border.width: 1 ; border.color: "lightgrey" }
}

