import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.5 as QC25
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import AnalysisModel 1.0
import "../../../../Global.js" as Global
import "../../../../controls" as Controls
import "../../../../datapanel/visualization" as Vis

Column {
	id : choiceLists
	height : parent.height
	spacing : 4
	Text {
		width : parent.width
		height : 25
		x : 4
		text : qsTr("Select Feature spaces")
		font.bold : true
		verticalAlignment : Text.AlignVCenter
	}
	Row {
		width : parent.width
		height : 130
		LayerList {
			id : xlayers
		}
		LayerList {
			id : ylayers
		}
	}
	Button {
		width : 200
		height : 22
		text : qsTr("Set Feature Space");
		onClicked : {
			var activePanel = layouts.activePanel()
			if ( activePanel) {
				activePanel.setBands(xlayers.selectedBand(), ylayers.selectedBand())
			}
		}
	}
	Row {

		id : buttons

		width : parent.width
		height : 25
		spacing : 4
		Item {
			width : 4
			height : parent.height
		}
		Controls.ToolButton{
			height : 20
			width : 20
			x: 164
			iconSource: "../../../../images/mapanels301.png"
			tooltip: qsTr("Splits the chart panels into several sub panels")
			onClicked: {
				switchLayout(1)
			}
		}
		Controls.ToolButton{
			height : 20
			width : 20
			x: 164
			iconSource: "../../../../images/mapanels30v2.png"
			tooltip: qsTr("Splits the chart panels into several sub panels")
			onClicked: {
				switchLayout(2)
			}
		}
		Controls.ToolButton{
			height : 20
			width : 20
			x: 164
			iconSource: "../../../../images/mapanels3021v.png"
			tooltip: qsTr("Splits the chart panels into several sub panels")
			onClicked: {
				switchLayout(3)
			}
		}
		Controls.ToolButton{
			height : 20
			width : 20
			x: 164
			iconSource: "../../../../images/mapanels304.png"
			tooltip: qsTr("Splits the chart panels into several sub panels")
			onClicked: {
				switchLayout(4)
			}
		}
	}
	ChartPanelLayout {
	    id : layouts
		width : parent.width
		height : parent.height - 200
	}

	function switchLayout(value){
	   if ( value === 1){
            layouts.source = "FeatureSpace1.qml"
       }
       if ( value ===2){
            layouts.source = "FeatureSpace2.qml"
       }
       if ( value === 3){
            layouts.source = "FeatureSpace12.qml"
       }
       if ( value === 4){
            layouts.source = "FeatureSpace22.qml"
       }
    }
	Component.onCompleted : {
		layouts.source = "FeatureSpace1.qml"
	}
}