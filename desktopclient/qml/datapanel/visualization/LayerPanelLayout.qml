import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Item {
    property alias source : panelLayout.source

    function activePanel(){
        if (!panelLayout.item)
            return null

        return panelLayout.item.activePanel()
    }


    Loader {
         id : panelLayout
         anchors.fill: parent

         onLoaded : {
             item.assignLayerManagers()
         }
    }
}