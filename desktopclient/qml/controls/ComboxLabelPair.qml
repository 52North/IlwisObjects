import QtQuick 2.6
import QtQuick.Controls 1.1
import QtQuick.Controls 2.4 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import "../controls" as Controls

Item {
    id : cbox   
    property string content
    property string labelText
    property int labelWidth
    property bool textEditable : false
    property int fontSize : 8
	property bool fontBold : true
    property bool readOnly : false;
    property alias itemModel : control.model
    property alias comboText : control.currentText
    property string initialComboText
	property bool transparentBackgrond : false
    property string role
    property alias currentIndex : control.currentIndex
    height : 28

    signal indexChanged()

    function select(name){
        var index = -1
        for(var i=0; i < control.model.length; ++i){
            if ( Global.compareStrings(control.model[i], name, true, true))    {
                index = i
                break
            }
        }
        control.currentIndex = index
    }
    
    onInitialComboTextChanged : {
        select(initialComboText)
    }

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: fontBold
        font.pointSize: fontSize
        y :11
    }
    QC2.ComboBox{
        id : control
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        model : itemModel
        editable : textEditable
        textRole: role

	    delegate: QC2.ItemDelegate {
		    id  : itdelg
			width: control.width
			height : 26
			contentItem: Text {
				text: modelData
				height : 26
				color: "black"
				font: control.font
				elide: Text.ElideRight
				verticalAlignment: Text.AlignVCenter
			}
			highlighted: control.highlightedIndex === index
			background: Rectangle {
				anchors.fill: itdelg
				color: itdelg.highlighted ? Global.selectedColor : "transparent"
			}
		}

		indicator: Canvas {
			id: canvas
			x: control.width - width - control.rightPadding
			y: control.topPadding + (control.availableHeight - height) / 2
			width: 12
			height: 8
			contextType: "2d"

			Connections {
				target: control
				onPressedChanged: canvas.requestPaint()
			}

			onPaint: {
				context.reset();
				context.moveTo(0, 0);
				context.lineTo(width, 0);
				context.lineTo(width / 2, height);
				context.closePath();
				context.fillStyle = control.pressed ? "#17a81a" : "#21be2b";
				context.fill();
			}
		}

		contentItem: Item {
		     width : control.width - 12
			 height : control.height - 12

			Text {
			    x : 4
				y : 11
				leftPadding: 0
				width : parent.width - 15
				rightPadding: control.indicator.width + control.spacing

				text: control.displayText
				font: control.font
				color: "black"
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight
				clip : true
			}
		}

		background: Rectangle {
		    id : backgr
			implicitWidth: 120
			implicitHeight: 40
			color : transparentBackgrond ? "transparent" : "white"
			Rectangle {
				width : parent.width - 15
				height: 2
				anchors.bottom : parent.bottom
				border.color: backgr.enabled ? "#d9d9d9" : "transparent"
				border.width: backgr.readOnly ? 0: 1

			}
		}

	    onCurrentIndexChanged: {
            indexChanged()
        }
    }
}



