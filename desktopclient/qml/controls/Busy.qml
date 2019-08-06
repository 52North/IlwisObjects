import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
	
BusyIndicator {
	anchors.fill : parent
	z : 100
	running : uicontext.uiBusy
	id :busy
}