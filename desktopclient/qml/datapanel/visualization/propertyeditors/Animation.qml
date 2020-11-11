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

	Column {
		x : 4
		y : 4
		width : 200
		height : 80
		spacing : 8
		Text {
			text : "Frame : " + editor.frameLabel
		}
		Controls.TextEditLabelPair {
			labelText : qsTr("Seconds per frame")
			labelWidth : 140
			width : 160
			content : "1"

			onContentChanged : {
				if ( content > 0 && !isNaN(content))
					timerid.interval = content * 1000
			}

		}
		 Button {
			width : 199
			height : 27
			text : qsTr(timerid.running ? "Stop" : "Run")

			onClicked :{
				timerid.running = !timerid.running;
			}
		 }
	 }

}
