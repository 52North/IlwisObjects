import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import "../datapanel/catalog" as Catalog
import "../Global.js" as Global
import "../controls" as Controls

Image {
        id : marker
		property var closeFunction 
		property var openFunction
        width : 20
		height : 20
        fillMode : Image.Pad
        source : state == "closed" ? "../images/bookmark_close.png" : "../images/bookmark_open.png"
        rotation: state == "closed" ? 0 : 90
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if ( marker.rotation == 90)    {
                    marker.state = "open"
                    marker.source = "../images/bookmark_open.png"
                }else{
                    marker.state = "closed"
                    marker.source = "../images/bookmark_close.png"
                }
            }
        }
        states: [
            State { name: "closed"

                PropertyChanges {
                    target: marker
                    z : 0
                    rotation : 90
                }
            },
            State {
                name : "open"
                PropertyChanges {
                    target: marker
                    rotation : 0
                }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation
                { id : opatrans; properties: "rotation"; duration : 300 ; easing.type: Easing.OutCubic
                }
				onRunningChanged: {
					if (!running){
						if ( state == "open")
							openFunction()
						if ( state == "closed")
							closeFunction()
					}
				}

            }
        ]
 }


