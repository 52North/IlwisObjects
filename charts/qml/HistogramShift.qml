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
import DataseriesModel 1.0

Column {
	x : 4
	y:  4
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation
	spacing : 4

	Controls.LabeledCheckBox {
		id : useshiftid
		x : 4
		y : 4
		labelText : qsTr("Activate")
		width : parent.width - 100
		labelWidth : 80

		onCheckedChanged : {
			chart.sendOverLink({"type" : "histogramshift", "resetstretch" : true})
			if ( control.value != 0 && useshiftid.checked)
				chart.sendOverLink({"type" : "histogramshift", "shift" : control.value})
			if ( useshiftid.checked) {
				operation.addIntensityCurve()
			}else
				operation.removeIntensityCurve()
		}
		Component.onCompleted : {
			checked = operation.isActive
		}
	}

	Row {
		width : Math.min(parent.width, 330)
		height :40
		spacing : 8
		visible : useshiftid.checked
		Text {
			width : 30
			height : Global.rowHeight
			text : -(operation.maxValue - operation.minValue)/2
			anchors.verticalCenter: parent.verticalCenter
		}
		QC2.Slider {
			id : control
			from: -(operation.maxValue - operation.minValue)/2
			value: 0
			to: (operation.maxValue - operation.minValue)/2
			height :parent.height
			width : 200

			handle: Rectangle {
				x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 26
				implicitHeight: 26
				radius: 13
				color: control.pressed ? Global.selectedColor : uicontext.lightestColor
				border.color: Global.edgecolor
				Text {
					anchors.centerIn : parent
					text : control.value.toFixed(0)
					font.pointSize : 8
				}
			}

			onPositionChanged : {
				operation.shiftValues(control.value)
				chart.sendOverLink({"type" : "histogramshift", "shift" : control.value})
			}
		}
		Text {
			width : 30
			height : Global.rowHeight
			text : (operation.maxValue - operation.minValue)/2
			anchors.verticalCenter: parent.verticalCenter
		}
	}
}

