import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

TextField {
    text: styleData.value
	property bool sel : false
	property var alt : styleData.row % 2 == 1
    verticalAlignment:Text.AlignVCenter
    font.pixelSize: 10

    style: TextFieldStyle {
		textColor: "black"
		background: Rectangle {
			implicitWidth: 100
			implicitHeight: 24
			border.color: control.focus ? Global.edgecolor : "transparent"
			border.width: 1
			color : control.sel ? Global.selectedColor : (control.alt? uicontext.lightestColor: "white")


		}
	}
}