import QtQuick 2.10
import QtQuick.Controls 1.2 as QC10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3
import "../Global.js" as Global

RoundButton {
	id : rplusbut
	property var backcolor : "transparent"
	property var noborder : true
	property var iconBase
    property bool entered : false

	contentItem: Image {
		source: rplusbut.iconBase + (entered ? "_black.svg" : "_green.svg")
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		width : parent.width
		height : parent.height
	}

	background: Rectangle {
		width: parent.height
		height: parent.width
		radius : 10
		color : backcolor
		border.color: noborder ? "transparent" : (rplusbut.down ? "grey" : "red")
		border.width: noborder ? 0 : 1
	}
}