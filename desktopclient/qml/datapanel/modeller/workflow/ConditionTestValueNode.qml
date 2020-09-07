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
    Layout.preferredWidth: 200
    Layout.preferredHeight: 50
    width: parent ? parent.width -84: Layout.preferredWidth
	//anchors.horizontalCenter : parent.horizontalCenter
	anchors.left : parent.left
	anchors.leftMargin : 4
	z : 1
    height: Layout.preferredHeight
	Rectangle {
		anchors.fill: parent
		radius: 6
		z: 1
		border.width: 1
		border.color : Global.edgecolor
		color : uicontext.lightestColor
		Column {
			y : 2
			id: background
			width : parent.width
			height : parent.height - 2
			spacing : 4
			Image {
				width : 20
				height : 20
				anchors.horizontalCenter : parent.horizontalCenter
				source : "../../../images/choice20.png" 
			}
			Text {
				width : Math.min(140, parent.width)
				height : 22
				anchors.horizontalCenter : parent.horizontalCenter

				text : "connect your condition here"
				elide : Text.ElideMiddle
			}
		}
	}
}