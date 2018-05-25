import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global

Item {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor

    TabView {
        anchors.fill : parent
   Tab {
            id : tab1
            title: "Pin Manager"
            CrossSectionPinManager{
                id : pinmanager
            }
    }
    Tab {
            title: "Data Source(s)"
            CrossSectionDataSources{}
        }
    }

    function handleMouseClick(mx,my){
        if ( tab1.item.selectedRow >= 0)
            editor.changeCoords(tab1.item.selectedRow, mx, my, true)
    }
}

