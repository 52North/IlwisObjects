import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls 2.4 as QC20
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Row {
	property var fmin
	property var fmax
	property var fresolution
    property string labelText
    property int labelWidth : 0
    property int fontSize : 8
    property bool readOnly : false;
    property bool transparentBackgrond : true
    property var regexvalidator
    property bool boldLabel : true
    property var checkFunction : null
	property bool showResolution : false

    height : Global.rowHeight
    width : parent.width
	spacing : 10

    signal contentEdited()

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.weight: boldLabel ? Font.Bold : Font.DemiBold
        font.pointSize: fontSize
        elide: Text.ElideMiddle
        y :3
        clip :true
    }
    TextField{
        id : textmin
        height : parent.height
        width : 50
        text : fmin
        font.pointSize: fontSize
        readOnly : parent.readOnly

        property string oldText

        style: TextFieldStyle {
            background: Rectangle {
                radius: 3
                width : parent.width
                height: parent.height
				color : transparentBackgrond ? "transparent" : "white"
				Rectangle {
					width : parent.width
					height: 2
					anchors.bottom : parent.bottom
					border.color: textmin.enabled ? "#d9d9d9" : "transparent"
					border.width: textmin.readOnly ? 0: 1

				}
            }
        }
        onTextChanged: {
            if ( regexvalidator){
                var regex = regexvalidator
                if (!regex.test(textmin.text)){
                    textmin.text = textmin.oldText
                }
                oldText = textmin.text
            }
        }

        onEditingFinished: {
            contentEdited()
        }
    }
    TextField{
        id : textmax
        height : parent.height
        width : 50
        text : fmax
        font.pointSize: fontSize
        readOnly : parent.readOnly

        property string oldText

        style: TextFieldStyle {
            background: Rectangle {
                radius: 3
                width : parent.width
                height: parent.height
				color : transparentBackgrond ? "transparent" : "white"
				Rectangle {
					width : parent.width
					height: 2
					anchors.bottom : parent.bottom
					border.color: textmax.enabled ? "#d9d9d9" : "transparent"
					border.width: textmax.readOnly ? 0: 1

				}
            }
        }
        onTextChanged: {
            if ( regexvalidator){
                var regex = regexvalidator
                if (!regex.test(textmax.text)){
                    textmax.text = textmax.oldText
                }
                oldText = textmax.text
            }
        }

        onEditingFinished: {
            contentEdited()
        }
    }

	TextField{
        id : textres
		visible : showResolution
        height : parent.height
        width : 50
        text : fresolution
        font.pointSize: fontSize
        readOnly : parent.readOnly

        property string oldText

        style: TextFieldStyle {
            background: Rectangle {
                radius: 3
                width : parent.width
                height: parent.height
				color : transparentBackgrond ? "transparent" : "white"
				Rectangle {
					width : parent.width
					height: 2
					anchors.bottom : parent.bottom
					border.color: textres.enabled ? "#d9d9d9" : "transparent"
					border.width: textres.readOnly ? 0: 1

				}
            }
        }
        onTextChanged: {
            if ( regexvalidator){
                var regex = regexvalidator
                if (!regex.test(textres.text)){
                    textres.text = textres.oldText
                }
                oldText = textres.text
            }
        }

        onEditingFinished: {
            contentEdited()
        }
    }
}

