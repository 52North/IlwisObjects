import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import "../Global.js" as Global
import "../controls" as Controls

Row {
    height : 20
	property var labelText
	property int labelWidth : 0
	property bool useWholeDays : false
	property var content : datetimepicker.selectedDate.toLocaleString(Qt.locale(),"yyyy-MM-dd" ) + (useWholeDays ?   '' :  "T" + timepicker.currentTime) 
	Text {
			y : 2
			width : labelWidth
			height : 20
			text : labelText
			font.bold : true
			visible : labelText != ""
	}
	Rectangle {
		id:comboBox
		width : 250
		height : Global.rowHeight + dropDown.height
		z: 100;
		smooth:true;
		color : "transparent"
		property int dropdownheight : 170

		Rectangle {
			id:chosenItem
			radius:2;
			width:parent.width;
			height:Global.rowHeight
			color: Global.alternatecolor3
			border.width: 1
			border.color: "#9494B8"
			smooth:true;
			Rectangle {
				id : choosenItemColor
				anchors.left: parent.left
				anchors.leftMargin: 5
				height: parent.height - 4
				width : 16
				//color : selectedColor ? selectedColor : "white"
				y : 2
			}

			Text {
				width: parent.width-50; height: parent.height
				anchors.left: parent.left; anchors.leftMargin: 25;
				verticalAlignment: Text.AlignVCenter
				id:chosenItemText
				font.family: "Arial"
				font.pixelSize: 12
				text : content
				smooth:true
			}

			Item {
				width: 20; height: 20
				anchors.right: parent.right
				Image {
					width: 16; height:16;
					anchors.centerIn: parent;
					source: "../images/down20.png"
					fillMode: Image.PreserveAspectFit
				}
			}

			MouseArea {
				anchors.fill: parent;
				onClicked: {
					comboBox.state = comboBox.state==="dropDown"?"":"dropDown"
				}
			}
		}

		Rectangle {
			id:dropDown
			width:comboBox.width;
			height:0;
			clip:true;
			radius:4;
			anchors.top: chosenItem.bottom;
			anchors.margins: 1;
			color: uicontext.workbenchBGColor 
			border.width: 1
			border.color: "#B0B0B0"

			Row {
				anchors.fill : parent
				Column{
					id : dtchoice
					width : 22
					height : dropDown.height

					Item {
						height : dropDown.height / 2
						width : parent.width
						AreaHeader { labeltext: "Date";source: "../images/tab_thin_active.png";fontPointSize : 9; height : dtchoice.width}
						MouseArea {
							anchors.fill : parent
							onClicked :{
								timepicker.visible = false
								datetimepicker.visible = true
							}
						}
					}

					Item {
						height : dropDown.height / 2
						width : parent.width
						visible : !useWholeDays
						AreaHeader { labeltext: "Time";source: "../images/tab_thin_active.png";fontPointSize : 9; height :  dtchoice.width}
						MouseArea {
							anchors.fill : parent
							onClicked :{
								timepicker.visible = true
								datetimepicker.visible = false
							}
						}
					}
				}
				Item {
					width : comboBox.width - dtchoice.width - 4
					height :  comboBox.dropdownheight - 4
					y : 2
					x : 2
					DateTimeCalendar {
						anchors.fill : parent			
						id : datetimepicker
						frameVisible : false
					}
					TimePicker{
					    id : timepicker
						anchors.fill : parent
						visible : false
					}
				}
			}
		}
		states: State {
			name: "dropDown";
			PropertyChanges { target: dropDown; height:comboBox.dropdownheight }
		}

		transitions: Transition {
			NumberAnimation { target: dropDown; properties: "height"; easing.type: Easing.OutExpo; duration: 400 }
		}
	}
}