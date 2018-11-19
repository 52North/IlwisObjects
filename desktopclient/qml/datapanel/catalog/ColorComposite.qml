import QtQuick 2.6
import QtQuick.Controls 1.1
import QtQuick.Controls 2.4 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Rectangle {
    width: parent.width - 5
    height: parent ? parent.height - 10 : 0
    property var operation

	Column {
	    id : bands
	    spacing : 4
	    width : parent.width
		height : 160
		x : 5

		Rectangle {
		    id : label
		    width : parent.width
			height : 30
			color : "#B9CEBD"
			Text {
				text : qsTr("Bands")
				width : parent.width
				font.bold : true
				height : 30
				y : 4
				x : 5
			}
		}

		Controls.TextEditLabelPair {
			labelWidth : 100
			labelText : qsTr("Red");
			width : Math.min(350, parent.width)
			height : 20
			content : operation.red

		}

		Controls.TextEditLabelPair {
			labelWidth : 100
			labelText : qsTr("Green");
			width : Math.min(350, parent.width)
			height : 20
			content : operation.green
		}

		Controls.TextEditLabelPair {
			labelWidth : 100
			labelText : qsTr("Blue");
			width : Math.min(350, parent.width)
			height : 20
			content : operation.blue

		}
		Column {
		    width : 300
			height : 65
			spacing : 3
			Text {
				font.bold : true
				text : qsTr("Side")
				width : 80
				height : 25
			}
			RowLayout {
				width : parent.width
				height : 20
				ExclusiveGroup { id: tabPositionGroup }
				RadioButton {
					id : b1
					text: qsTr("Left")
					width : 75
					exclusiveGroup: tabPositionGroup
				}
				RadioButton {
					id : b2
					text: qsTr("Right")
					width : 75
					exclusiveGroup: tabPositionGroup
				}
				RadioButton {
					id : b3
					text: qsTr("Other")
					checked : true
					width : 75
					exclusiveGroup: tabPositionGroup
				}
			}
		}

	}

	Button{
		width : 100
		height : 22
		text : "Apply"
		anchors.top : bands.bottom
		anchors.topMargin : 8

		onClicked : {
			var ids = mastercatalog.selectedIds();
			console.debug("zzzzzzzzzzz", ids)
			if ( ids != "") {
				var side = "other"
				if (b1.checked)
					side = "left"
				if ( b2.checked)
					side = "right"
				var parms = { 'side' : side, 'rasters' : ids}
				operation.execute(parms)
			}
		}
	}
}