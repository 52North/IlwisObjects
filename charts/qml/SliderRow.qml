import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls 2.5 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml" as Base
import ChartModel 1.0
import DataseriesModel 1.

Row { 
	property var minV : 0
	property var maxV : 255
	property var minValue : -1000000
	property var currentValue : control1.value
	property var startValue : -1
	property var labelText : "" 


	onMinValueChanged : {
		if ( minValue > control1.value)
			control1.value = minValue
	}

	width : Math.min(parent.width, 330)
	height :30
	spacing : 4
	visible : stretchtooltid.checked
	Text {
		id : label
		width : 60
		height : Global.rowHeight
		text : labelText
		anchors.verticalCenter: parent.verticalCenter
		visible : labelText != "" 
	}

	Text {
		width : 30
		height : Global.rowHeight
		text : minV.toFixed(2)
		anchors.verticalCenter: parent.verticalCenter
	}
	QC2.Slider {
		id : control1
		from: minV
		value: startValue
		to: maxV
		height :parent.height
		width : 200 

		handle: Rectangle {
			x: control1.leftPadding + control1.visualPosition * (control1.availableWidth - width)
			y: control1.topPadding + control1.availableHeight / 2 - height / 2
			implicitWidth: 26
			implicitHeight: 26
			radius: 13
			color: control1.pressed ? Global.selectedColor : uicontext.lightestColor
			border.color: Global.edgecolor
			Text {
				anchors.centerIn : parent
				text : control1.value.toFixed(2)
				font.pointSize : 6
			}
		
		}

		onPositionChanged : {
			if ( value < minValue)
				value = minValue
		}

		onPressedChanged : {
			if(!pressed)
				handleReleased(control1.value)
		}
	}
	Text {
		width : 30
		height : Global.rowHeight
		text : maxV.toFixed(2)
		anchors.verticalCenter: parent.verticalCenter
	}
}