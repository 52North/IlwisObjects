import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls 2.12 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Item {
    width: 100
    height: 62
    property var editor

	  Timer {
		id : timerid
        interval: 1000; running: false; repeat: true
        onTriggered: 
			{
				editor.updateAnimation(1)
			}
	  }

	 Controls.PushButton {
		width : 199
		height : 22
		text : qsTr("Run")

		onClicked :{
			timerid.running = !timerid.running;
		}
	 }

}
