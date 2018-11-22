import QtQuick 2.10
import QtQuick.Controls 1.2 as QC10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3
import DataPaneModel 1.0
import TabModel 1.0
import "Global.js" as Global
import "./controls" as Controls

MenuItem {
	id: menuItem
	implicitWidth: itemWith
	implicitHeight: 24
	property var actionFunc
	
	arrow: Canvas {
		x: parent.width - width
		implicitWidth: 40
		implicitHeight: 24
		visible: menuItem.subMenu
		onPaint: {
			var ctx = getContext("2d")
			ctx.fillStyle = menuItem.highlighted ? Global.selectedColor : "green"
			ctx.moveTo(15, 4)
			ctx.lineTo(width - 15, height / 2)
			ctx.lineTo(15, height - 4)
			ctx.closePath()
			ctx.fill()
		}
	}

	indicator: Item {
		implicitWidth: 40
		implicitHeight: 24
		Rectangle {
			width: 26
			height: 26
			anchors.centerIn: parent
			visible: menuItem.checkable
			border.color: "darkgrey"
			radius: 3
			Rectangle {
				width: 14
				height: 14
				anchors.centerIn: parent
				visible: menuItem.checked
				color: "#21be2b"
				radius: 2
			}
		}
	}

	contentItem: Row {
		height : 24
		width : parent.width
		id : textrow
		spacing : 3
		RoundButton {
			id : button1
			width : 20
			height : 20
			text : "L"
			visible: menuItem.subMenu == null
			anchors.verticalCenter :  textrow.verticalCenter

			background: Rectangle {
				implicitWidth: 20
				implicitHeight: 20
				color: button1.down ? "#d6d6d6" : "transparent"
				border.color: "darkgrey"
				border.width: 1
				radius: parent.radius

			}

			onClicked : {
				actionFunc(menuItem.text, "left",menuItem.menu)

			}
		}
		Text {
			width : parent.width - 45
			height : 24
			text: menuItem.text
			font: menuItem.font
			opacity: enabled ? 1.0 : 0.3
			color: menuItem.highlighted ? "blue" : "black"
			horizontalAlignment: Text.AlignLeft
			verticalAlignment: Text.AlignVCenter
			elide: Text.ElideRight
			anchors.verticalCenter :  textrow.verticalCenter
		}
		RoundButton {
		    id : button2
			width : 20
			height : 20
			//flat : true
			text : "R"
			visible: menuItem.subMenu == null

			anchors.verticalCenter :  textrow.verticalCenter

				background: Rectangle {
				implicitWidth: 20
				implicitHeight: 20
				color: button2.down ? "#d6d6d6" : "transparent"
				border.color: "darkgrey"
				border.width: 1
				radius: parent.radius

			}
			onClicked : {
				actionFunc(menuItem.text, "right",menuItem.menu)
			}
		}
	}

	background: Rectangle {
		implicitWidth: itemWith
		implicitHeight: 24
		opacity: enabled ? 1 : 0.3
		color: menuItem.highlighted ? Global.selectedColor : "transparent"
	}
}
