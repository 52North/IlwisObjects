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
	property var test : 0

	  Timer {
		id : timerid
        interval: 1000; running: false; repeat: true
        onTriggered: 
			{
				console.debug("zzzzzzzzzz0", test, " --> ",Date().toString())
				editor.updateAnimation(1)
				++test

			}
	  }

	 Button {
		width : 199
		height : 27
		text : qsTr("Run")

		onClicked :{
			timerid.running = !timerid.running;
			//editor.updateAnimation(1)
			//++test
			//console.debug("zzzzzzzzzzzzzz", test)
		}
	 }

}
