import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "qrc:/qml/controls" as Controls

Column {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor
	spacing : 4

	Controls.ComboxLabelPair {
	    id : charttype
            labelText : qsTr("Text Position")
            labelWidth : 100
            width : 300
            height : 20
            itemModel : ["No text", "Top Left", "Middle Left", "Bottom Left", "Top Center", "Bottom Center", "Top Right", "Middle Right", "Bottom Right"] 
	}
	Controls.PushButton {
		text : qsTr("Apply")
		width : 100
		height : 22
	}
	
}
