import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan

import "../../../Global.js" as Global
import "../../../controls" as Controls

Qan.NodeItem {
	id : rangeNode
    Layout.preferredWidth: 200
    Layout.preferredHeight: 50
    width: parent ? parent.width -105: Layout.preferredWidth
	//anchors.horizontalCenter : parent.horizontalCenter
	//anchors.left : parent.left
	//anchors.leftMargin : 4
	z : 1
    height: Layout.preferredHeight
	Rectangle {
		id: testRectangle

		anchors.fill: parent
		radius: 6
		z: 1
		border.width: 1
		border.color : Global.edgecolor
		color : uicontext.lightestColor

		Row {
			width : parent.width - 4
			height : parent.height
			x : 2
			y : 8
			TextField {
				id : rangeDefinition
				width : parent.width - marker.width
				height : parent.height - 20
				text : node.rangeValue
				y : 1
				z : 2
			}


			Button {
				id : marker
				width : 28
				height : parent.height - 20
				y : 1
				text : "Set"
				onClicked: {
					rangeNode.node.setRangeTestValues(rangeDefinition.text)
					workflowManager.updateRunForm()
				}
			}
		}
	}
	function setTests() {
		var node = workflow.getNode(rangeItem.itemid)
		if ( node){
			rangeDefinition.text = node["rangedefinition"]
		}
	}

	function setTest(txt){
		rangeDefinition.text = txt;
	}


	function addTestOperation(objectid, url, typename){
			var object = mastercatalog.id2object(objectid,testRectangle)
		if (object)
			rangeDefinition.text = object.shortRangeDefinition()
	}

	function storeRangeDefinition() {
		console.debug("stored")
		workflow.setRangeTestValues(rangeItem.itemid, rangeDefinition.text)
	}
}
