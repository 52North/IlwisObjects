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
    orientation: Qt.Horizontal
    
	FeatureSpaceDelegate {
		id : fs1
	    width : parent.width / 2
        height : parent.height
		fsIndex : 1

	}

    Item{
        width : parent.width / 2
        height : parent.height

		SplitView {
			anchors.fill : parent
			orientation: Qt.Vertical

			FeatureSpaceDelegate {
				id : fs2
				width : parent.width 
				height : parent.height/2
				fsIndex :2

			}

			FeatureSpaceDelegate {
				id : fs3
				width : parent.width 
				height : parent.height/2
				fsIndex :3
			}
		}
    }
	function activePanel() {
		return fs1.active ? fs1 : (fs2.active ? fs2 : fs3);
	}

	function deActivateAll(){
		fs1.active = false
		fs2.active = false
		fs3.active = false
	}

	Component.onCompleted : {
		fs1.active = true
		fs2.active = false
		fs3.active = false;

		fs1.setBands(selectedXBand(), selectedYBand())
		fs2.setBands(selectedXBand(), selectedYBand())
		fs3.setBands(selectedXBand(), selectedYBand())
	}


	function updateFS(){
		fs1.setBands(selectedXBand(), selectedYBand())
		fs1.updateChart2() 
		fs2.setBands(selectedXBand(), selectedYBand())
		fs2.updateChart2() 
		fs3.setBands(selectedXBand(), selectedYBand())
		fs3.updateChart2() 
	}
}