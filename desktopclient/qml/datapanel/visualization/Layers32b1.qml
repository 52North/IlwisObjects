import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

SplitView {
    orientation: Qt.Horizontal
    
    function activePanel(){
        if ( layerview.activeSubPanel == 0)
            return lyrUp
        if ( layerview.activeSubPanel == 1)
            return lyrDown
        return lyrRight
    }

    function assignLayerManagers() {
        lyrUp.assignLayerManager(layerview.layermanagers[0])        
        lyrDown.assignLayerManager(layerview.layermanagers[1])  
        lyrRight.assignLayerManager(layerview.layermanagers[2])  
    }

    Item {
        width : parent.width / 2
        height : parent.height
        SplitView {
            anchors.fill : parent
            orientation: Qt.Vertical
            Layers{
                id : lyrUp
                width : parent.width / 2
                height : parent.height / 2
                panelSubIndex : 0
            }

            Layers{
                id : lyrDown
                width : parent.width / 2
                height : parent.height / 2
                panelSubIndex : 1
            }
        }
    }
    Layers{
        id : lyrRight
        width : parent.width / 2
        height : parent.height
        panelSubIndex : 2
    }


}