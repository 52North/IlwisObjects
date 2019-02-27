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
	Column {
	    id : choiceLists
	    width : parent.width
		height : 190
		spacing : 4
		Text {
			width : parent.width
			height : 25
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
		Row {

			id : buttons

			width : parent.width
			height : 24
			spacing : 4
			Item {
				width : 4
				height : parent.height
			}
			Button {
				width : 100
				height : 20
				text : qsTr("Add")
			}
			Controls.ToolButton{
				height : 20
				width : 20
				x: 164
				iconSource: "../../../../images/mappanels20.png"
				tooltip: qsTr("Splits the chart panels into several sub panels")
				onClicked: {
					layoutChoices.x = x
					layoutChoices.toggle()
				}
				z : 30
			}
			SubPanelBox {
				id : layoutChoices
				y : 18
				x : 145
				height : 20
				state : "invisible"
				clip:true
				z : 1000
				panelCallBack : switchLayout
			}
		}
	}
	ListView {
		width : parent.width
		height : parent.height - choiceLists.height
	}

	function switchLayout(type){
    }
}