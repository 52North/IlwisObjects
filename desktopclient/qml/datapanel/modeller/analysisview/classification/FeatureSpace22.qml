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

   	SplitView {
		width : parent.width
		height : parent.height / 2
		orientation: Qt.Horizontal

		FeatureSpaceDelegate {
				id : fs1
				width : parent.width / 2
				height : parent.height
				fsIndex : 1

		}

		FeatureSpaceDelegate {
				id : fs2
				width : parent.width / 2
				height : parent.height
				fsIndex : 1

		}
	}

	SplitView {
		width : parent.width
		height : parent.height / 2
		orientation: Qt.Horizontal

		FeatureSpaceDelegate {
				id : fs3
				width : parent.width / 2
				height : parent.height
				fsIndex : 1

			}

		FeatureSpaceDelegate {
				id : fs4
				width : parent.width / 2
				height : parent.height
				fsIndex : 1

		}
	}
	function activePanel() {
		if ( fs1.active) return fs1
		if ( fs2.active) return fs2
		if ( fs3.active) return fs3
		if ( fs4.active) return fs4
	}

	function deActivateAll(){
		fs1.active = false
		fs2.active = false
		fs3.active = false
		fs4.active = false
	}

	Component.onCompleted : {
		fs1.active = true
		fs2.active = false
		fs3.active = false
		fs4.active = false

		fs1.setBands(selectedXBand(), selectedYBand())
		fs2.setBands(selectedXBand(), selectedYBand())
		fs3.setBands(selectedXBand(), selectedYBand())
		fs4.setBands(selectedXBand(), selectedYBand())
	}
	function updateFS(){
		fs1.setBands(selectedXBand(), selectedYBand())
		fs1.updateChart2() 
		fs2.setBands(selectedXBand(), selectedYBand())
		fs2.updateChart2() 
		fs3.setBands(selectedXBand(), selectedYBand())
		fs3.updateChart2() 
		fs4.setBands(selectedXBand(), selectedYBand())
		fs4.updateChart2()
	}
}