import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import "../Global.js" as Global
import "../controls" as Controls

Item {
	anchors.fill : parent
	CircularSlider {
		id: outer

		height : 165
		numberOfSteps : 12
		maximumValue : 24
		borderImage : "../images/circularSlider.png"
		markerImage : "../images/circularSliderHandle.png"
		x : 40

		CircularSlider {
		id : inner
			height : 105
			numberOfSteps : 60
			maximumValue : 60
			borderImage : "../images/circularSlider.png"
			markerImage : "../images/circularSliderHandle.png"
			x : 30
			y : 29
		}

	}
}