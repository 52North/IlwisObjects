import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.5
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../.." as Base
import "../../../../qml/datapanel" as DataPanel

Rectangle {
    id : chartManagement
    width: parent.width
    Layout.minimumHeight: 24

    TabView {
        anchors.fill: parent
        id : charttabs
        tabPosition: Qt.BottomEdge

        Tab {
            title : qsTr("Actions for Charts")
            SplitView {
                id : dataseriesview
	            property string iconName : "../../qml/images/actions"

                handleDelegate: Controls.SplitHandle {
                    imageHeight: 15
                }

                DataseriesList {
                    id : dataserieslist
                }
                DataseriesOperationList {
                    id : dataseriesOperationList
                }
                DataseriesOperate {
                    id : dataseriesoperation
                    Layout.fillWidth: true
                }
            }

        }

       Tab {
            title : qsTr("Metadata")
            id : columnmetadataview
			Item {
	            property string iconName : "../../qml/images/metadata"
			}

        }

		Component.onCompleted : {
			columnmetadataview.active = true
		}
 
        style: DataPanel.ButtonBarTabViewStyle{}

        function tabClicked(index){
            if ( currentIndex === index){
                if ( chartManagement.height <= 60){
                    chartspanel.state = "visible"
                }
                else{
                    chartspanel.state = ""
                    chartspanel.state = "invisible"
                }
            }

            currentIndex = index
        }

    }
}

