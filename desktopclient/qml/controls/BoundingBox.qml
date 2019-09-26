import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Controls 2.3 as QC2
import UIContextModel 1.0
import "../Global.js" as Global
import "../controls" as Controls

Row {
	property var label : qsTr("Bounding Box")
	property var labelWidth : 100
	property var content : getContent()
	width : labelWidth + 140 + 20

	spacing : 2

	Text {
		height : 20
		width : labelWidth

		text : label
	}
	
	Controls.TextEditLabelPair {
		id : left
		width : 60
		height : 20
		boldLabel : false
		labelText : qsTr("x1")
		labelWidth : 15
	}

	Controls.TextEditLabelPair {
		id : up 
		width : 60
		height : 20
		boldLabel : false
		labelText : qsTr("y1")
		labelWidth : 15
	}

	Controls.TextEditLabelPair {
		id : right
		width : 60
		height : 20
		boldLabel : false
		labelText : qsTr("x2")
		labelWidth : 15
	}

	Controls.TextEditLabelPair {
		id : down
		width : 60
		height : 20
		boldLabel : false
		labelText : qsTr("y2")
		labelWidth : 15
	}

	function getContent() {
		return left.content + " " + up.content + "," + right.content + " " + down.content
	}
	
}