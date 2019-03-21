import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

SplitView {
    orientation: Qt.Vertical

	FeatureSpaceDelegate {
		id : fs1
	    width : parent.width
        height : parent.height / 2
		fsIndex : 0

	}

	FeatureSpaceDelegate {
		id : fs2
	    width : parent.width
        height : parent.height / 2
		fsIndex : 1

	}
	function activePanel() {
		return fs1.active ? fs1 : fs2;
	}

	function deActivateAll(){
		fs1.active = false
		fs2.active = false
	}

	Component.onCompleted : {
		fs1.active = true
		fs2.active = false

		fs1.setBands(selectedXBand(), selectedYBand())
		fs2.setBands(selectedXBand(), selectedYBand())
	}

	function updateFS(){
		fs1.setBands(selectedXBand(), selectedYBand())
		fs2.setBands(selectedXBand(), selectedYBand())
	}
}