import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4
import "../Global.js" as Global
import "../controls" as Controls

Item {
    property var currentTime : time.text

	anchors.fill : parent
	Rectangle {
	    id : box
		width : parent.width - 40
		height : parent.height
		x : 40

		CircularSlider {
			id: outer

			height : box.height
			numberOfSteps : 12
			maximumValue : 12
			overflowOffset : 12
			overflowCount : 1
			ringColor : Global.middlegreen
			ringWidth : 58
			markerImage : "../images/circularSliderHandle.png"
			anchors.centerIn : parent 
			
		}
		CircularSlider {
			id : inner
			height : 100
			numberOfSteps : 60
			maximumValue : 60
			markerImage : "../images/circularSliderHandle.png"
			ringColor : Global.lightgreen
			ringWidth : 40
			anchors.centerIn : box
		}
		Text {
			id : time
			anchors.centerIn : inner
			//anchors.verticalCenterOffset : -10
			text : outer.currentValue + ":" + box.leftPad(inner.currentValue,2)
			font.pointSize : 12
			font.bold : true
		}
		function leftPad(number, targetLength) {
			var output = number + '';
			while (output.length < targetLength) {
				output = '0' + output;
			}
			return output;
		}
	}
}