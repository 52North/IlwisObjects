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

	contentItem: Text {
		text: rplusbut.text
		font: rplusbut.font
		opacity: enabled ? 1.0 : 0.3
		color: rplusbut.down ? "black" : "grey"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: Rectangle {
		implicitWidth: parent.height
		implicitHeight: parent.width
		radius : parent.height
		opacity: enabled ? 1 : 0.3
		color : backcolor
		border.color: noborder ? "transparent" : (rplusbut.down ? "grey" : "lightgrey")
		border.width: noborder ? 0 : 1
	}
}