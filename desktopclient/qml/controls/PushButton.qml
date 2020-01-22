import QtQuick 2.12
import QtQuick.Controls 2.2 as QC2
import "../Global.js" as Global

QC2.Button{
	id : control
	property bool noEdge : true

	background: Rectangle {
		implicitWidth: parent.width
		implicitHeight: parent.height
		color: parent.down ? uicontext.paleColor :  ( highlighted ? uicontext.middleColor :uicontext.lighterColor) //"#f6f6f6"
		border.color: Global.edgecolor
		border.width: noEdge ? 0 : 1
		radius: 4
	} 
   /* contentItem: QC2.Label { 
        text: control.text
        font: control.font
        verticalAlignment: Text.AlignVCenter
    }*/
	/*icon.source : "../images/plus20.png"
	icon.width : 22
	icon.height : 22
	icon.color : "transparent"*/

}