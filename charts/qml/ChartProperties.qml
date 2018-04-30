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
                    chartpanel.state = "visible"
                }
                else{
                    chartpanel.state = ""
                    chartpanel.state = "invisible"
                }
            }

            currentIndex = index
        }
        Tab {
            title : qsTr("Data Series")
           // property string iconName : "../images/info_s"
        }

       Tab {
            title : qsTr("Meta data")
            property string iconName : "../images/metadata_s"
            id : columnmetadatview

        }
 
       style: DataPanel.ButtonBarTabViewStyle{}
    }
}

