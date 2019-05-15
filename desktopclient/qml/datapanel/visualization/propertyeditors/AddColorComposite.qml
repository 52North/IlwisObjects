import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor

	Column {
	    id : bands
	    spacing : 4
	    width : parent.width
		height : 160
		x : 5

		Rectangle {
		    id : label
		    width : parent.width
			height : 30
			color : "#B9CEBD"
			Text {
				text : qsTr("Bands")
				width : parent.width
				font.bold : true
				height : 30
				y : 4
				x : 5
			}
		}

		Controls.TextEditLabelPair {
		    id : red
			labelWidth : 100
			labelText : qsTr("Red");
			width : Math.min(350, parent.width)
			height : 20
			checkFunction: testDrop
		}

		Controls.TextEditLabelPair {
			id : green
			labelWidth : 100
			labelText : qsTr("Green");
			width : Math.min(350, parent.width)
			height : 20
			checkFunction: testDrop
		}

		Controls.TextEditLabelPair {
			id : blue
			labelWidth : 100
			labelText : qsTr("Blue");
			width : Math.min(350, parent.width)
			height : 20
			checkFunction: testDrop
		}
	}

	Button{
		width : 100
		height : 22
		text : "Apply"
		anchors.top : bands.bottom
		anchors.topMargin : 8

		onClicked : {
			editor.apply(red.content, green.content, blue.content)
		}
	}
	function testDrop(id){
        if (!id)
            return false
		var ok = false
        var obj = mastercatalog.id2object(id, null)
        if ( obj) {
			if (obj.typeName === "rastercoverage"){
			    var isint = Global.isInteger(obj.internalValuetype)
				if ( isint){
						ok = true
					}
			}
			obj.suicide()
        }
        return ok
    }
}

