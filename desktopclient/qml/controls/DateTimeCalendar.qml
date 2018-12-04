	import QtQuick 2.5
	import QtQuick.Controls 1.4
	import QtQuick.Layouts 1.1
	import QtQuick.Controls.Styles 1.1
	import InternalDatabaseModel 1.0
	import QtGraphicalEffects 1.0
	import "../Global.js" as Global
	import "../controls" as Controls

Item {
	property alias frameVisible  : calendarInternal.frameVisible
	property alias selectedDate : calendarInternal.selectedDate
	Calendar {
		id : calendarInternal
		height : parent.height
		width : parent.width


		style: CalendarStyle {
			gridVisible: false
			dayDelegate: Rectangle { width : 40;height : 16;color : styleData.selected ? Global.selectedColor : "white"
				Label {
					text: styleData.date.getDate()
					anchors.centerIn: parent
					font.bold : styleData.selected ? true : false
				}
			}
			dayOfWeekDelegate : Rectangle { width : 40;height : 20; color : Global.lightergreen
				Label {
					text: control.locale.dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
					anchors.centerIn: parent
					color: "grey"
				}
			}

			navigationBar : 
			Rectangle {
				height: 25
				color: Global.lightestgreen

				Rectangle {
					color: Qt.rgba(1,1,1,0.6)
					height: 1
					width: parent.width
				}

				Rectangle {
					anchors.bottom: parent.bottom
					height: 1
					width: parent.width
					color: "#ddd"
				}
				Button {
					id: previousMonth
					width: parent.height
					height: width
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					//iconSource: "../images/arrowleft.png"
					style: ButtonStyle {
						label : Text{
							x : 2
							y : -2

							//anchors.centerIn : parent
							text : "<"
							font.pointSize : 11
						}
					}
					onClicked: control.showPreviousMonth()
				}
				Label {
					id: dateText
					text: styleData.title
					elide: Text.ElideRight
					horizontalAlignment: Text.AlignHCenter
					font.pointSize : 9
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: previousMonth.right
					anchors.leftMargin: 2
					anchors.right: nextMonth.left
					anchors.rightMargin: 2
				}
				Button {
					id: nextMonth
					width: parent.height
					height: width
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right
					style: ButtonStyle {
						label : Text{
							x : 2
							y : -2

							//anchors.centerIn : parent
							text : ">"
							font.pointSize : 11
						}
					}
					onClicked: control.showNextMonth()
				}
			}
		}
	}
}