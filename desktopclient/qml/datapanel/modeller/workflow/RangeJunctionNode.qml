import QtQuick              2.7
import QtQuick.Controls     2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts      1.3
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects   1.0
import OperationModel 1.0
import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan
import "../../../Global.js" as Global
import "../../../controls" as Controls

Qan.NodeItem {
    Layout.preferredWidth: 50
    Layout.preferredHeight: Layout.preferredWidth
    width: 50
	height: width

	Item {
	id : innerItem
	anchors.fill: parent
	Rectangle {
		id: background
		z: 1
		width :parent.width; height : width
		radius: width / 2
		border.width: 1
		border.color : "green"

		Image {
			id : iconid
			width : 20
			height : 20
			anchors.centerIn : parent
			source : "../../../images/junction.png" 
		}
	}
}
}