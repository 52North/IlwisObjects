import QtQuick 2.12
import QtQuick.Controls 1.1
import QtQuick.Controls 2.12 as QC2
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
	property bool fontBold : false
    property bool readOnly : false;
    property alias itemModel : control.model
    property alias comboText : control.currentText
    property string initialComboText
	property bool transparentBackgrond : false
    property string role
    property alias currentIndex : control.currentIndex
	property var callback
    height : 20

    signal indexChanged()

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
        y :10
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
			height : 25
			contentItem: Text {
				text: stripName(control.textRole ? modelData[control.textRole] : modelData)
				height : 20
				color: "black"
				font.bold : isSpecial(control.textRole ? modelData[control.textRole] : modelData) 
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
				y : 5
				leftPadding: 0
				width : parent.width - 15
				rightPadding: control.indicator.width + control.spacing

				text: stripName(control.displayText)
				font: control.font
				color: "black"
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight
				clip : true

				onTextChanged : {
					if ( callback)	
						callback(text)
				}
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

		popup: QC2.Popup {
		    id : cbpopup
			y: control.height - 1
			width: control.width
			height : Math.min(control.delegateModel.count * 26, 300)

			padding: 1

			contentItem: ScrollView{
					clip: true
					height : cbpopup.height
					ListView {
						clip: true
						height : parent.height
						model: control.popup.visible ? control.delegateModel : null
						currentIndex: control.highlightedIndex

						QC2.ScrollIndicator.vertical: QC2.ScrollIndicator { }
					}
			}

        background: Rectangle {
            border.color: "#21be2b"
            radius: 2
        }
    }

        onActivated : {
            indexChanged()
        }


    }
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

	function stripName(txt) {
		if ( txt.charAt(0) == "*")
			return txt.substr(1)
		return txt

	}

	function isSpecial(txt) {
		return txt.charAt(0) == "*"

	}
}



