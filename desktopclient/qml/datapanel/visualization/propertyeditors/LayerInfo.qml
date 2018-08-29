import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import IlwisObjectModel 1.0
import "../../../Global.js" as Global
import "../../../Controls" as Controls

Column {
    id : layerInfoPanel
    width : parent ? parent.width - 15 : 0
    height : Global.rowHeight + 20
    property var editor
	property var coverage

    onEditorChanged : {
		coverage = editor.object(layerInfoPanel)
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : pathLabel
			width : 70
			height : 22
			text : qsTr("Path")
		}
		Text {
			width : parent.width - pathLabel.width - openBut.width 
			height : 22
			text : coverage ? coverage.url : ""
		}
		Button {
		id : openBut
			iconName : "../../../images/openCS1.png"
			width : 22
			height : 22
			onClicked : {
				bigthing.newCatalog("container='" + coverage.container +"'","catalog",coverage.container, "other")
			}
		}
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : tpLabel
			width : 70
			height : 22
			text : qsTr("Type")
		}
		Text {
			width : parent.width - tpLabel.width
			height : 22
			text : coverage ? coverage.typeName : ""
		}
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : prjLabel
			width : 70
			height : 22
			text : qsTr("Projection")
		}
		Text {
			width : parent.width - prjLabel.width
			height : 22
			text : coverage ? coverage.projectionInfo : ""
		}
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : domainLabel
			width : 70
			height : 22
			text : qsTr("Domain")
		}
		Text {
			width : parent.width - domainLabel.width
			height : 22
			text : coverage ? coverage.domainName : ""
		}
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : domainType
			width : 70
			height : 22
			text : qsTr("Domain Type")
		}
		Text {
			width : parent.width - domainType.width
			height : 22
			text : coverage ? coverage.domainType : ""
		}
	}

	Row {
		width : parent.width - 15
		height : 22
		Text {
			id : valueType
			width : 70
			height : 22
			text : qsTr("Value Type")
		}
		Text {
			width : parent.width - valueType.width
			height : 22
			text : coverage ? coverage.valuetype : ""
		}
	}
}