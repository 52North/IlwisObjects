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

Rectangle {
	width : parent.width/2
	height : parent.height
	border.width : 1

	border.color : Global.edgecolor
		ScrollView{
		id : control
        anchors.fill : parent
		ListView {
		    id : list
			anchors.fill : parent
			model : bands ? bands : null
			highlight : Rectangle{width : parent.width;height : 18;color : Global.selectedColor}
			delegate : Text {
				x : 5
				height : 18
				width : parent.width - 5
				text : modelData.name
				MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        list.currentIndex = index
                    }
                }
			}
		}
	}
	
	function selectedBand() {
		return list.currentIndex
	}
}