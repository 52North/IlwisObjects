import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../../Global.js" as Global

Item {
    id : panels
    property alias source : panelLayout.source

	function setSource(url, parms){
		panelLayout.setSource(url, parms)
	}

    function activePanel(){
        if (!panelLayout.item)
            return null

        return panelLayout.item.activePanel()
    }


    Loader {
         id : panelLayout
         anchors.fill: parent
    }
    states: [
        State { name: "visible"

            PropertyChanges {
                target: panels
                height : parent.height -  Global.actionBarMaxHeight
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: panels
                height : parent.height - activeLayerExtentsToolbar().height + 10
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]
}