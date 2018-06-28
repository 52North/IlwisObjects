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

    TabView{
        anchors.fill: parent
        id : charttabs
        tabPosition: Qt.BottomEdge

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
        Tab {
            title : qsTr("Operations")
            SplitView {
                id : dataseriesview
	            property string iconName : "../../qml/images/graph"

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
            title : qsTr("Meta data")
            property string iconName : "../../qml/images/metadata_s"
            id : columnmetadatview

        }
 
       style: DataPanel.ButtonBarTabViewStyle{}
    }
}

