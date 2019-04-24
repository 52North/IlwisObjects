import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Controls 2.3 as QC2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "." as Charts
import ChartModel 1.0
import TabModel 1.0
import TableModel 1.0
import "../../../../qml" as Base
import "../../../../qml/datapanel/table" as TablePanel
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls

Row {
    width : parent.width
    height : 35
	property bool zoomMode : false

	Button {
		width : 35
		height : 25
		Image {
			width : 20
			height : 20
			source : "../../../../qml/images/full_green.svg" 
			anchors.centerIn : parent
		}
		onClicked : {
			zoomMode = false
			zoomReset()
		}

	}

	Button {
		width : 35
		height : 25
		Image {
			width : 20
			height : 20
			source : "../../../../qml/images/zoomin_green.svg" 
			anchors.centerIn : parent
		}
		onClicked : {
			zoomMode = true
		}

	}
}