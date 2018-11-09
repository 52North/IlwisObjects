import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import StartIlwis 1.0

ApplicationWindow {
    width: 543
    height: 401
    title: qsTr("Ilwis 4")
    id : rootwindow
    visible: true
	flags:  Qt.Window  | Qt.FramelessWindowHint
    color: "transparent"
	objectName : "mainwindow__mainui"
	property bool initDone : false

	Component {
		id : dummy
		Rectangle {
			width : 543
			height : 401
			color : "blue"
		}
	}

    function initIlwis() {
        startilwis.init()
		windowloader.item.time = 500
		windowloader.item.state = "begin"
    }

	function showMain() {
	        rootwindow.flags = Qt.Window
			rootwindow.x = 0.1 *Screen.width
			rootwindow.y = 0.05 * Screen.height
			rootwindow.width = Screen.width * 0.8
			rootwindow.height = Screen.height * 0.9

			windowloader.source = "DesktopClient.qml"
			startilwis.initMainWindow()
			color = uicontext.lightColor
	}

    Loader{
        id : windowloader
    }

	Component.onCompleted :{
		windowloader.source = "Splash.qml"
	}
	onClosing: {
        uicontext.exitUI()
    }
}
