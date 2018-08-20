import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

SplitView {
    orientation: Qt.Vertical
    
    function activePanel(){
        if ( layerview.activeSubPanel == 0)
            return lyrLeft
        if ( layerview.activeSubPanel == 1)
            return lyrRight
        return lyrDown
    }

    function assignLayerManagers() {
        lyrLeft.assignLayerManager(layerview.layermanagers[0])        
        lyrRight.assignLayerManager(layerview.layermanagers[1])  
        lyrDown.assignLayerManager(layerview.layermanagers[2])  
    }

    Item {
        width : parent.width
        height : parent.height / 2
        SplitView {
            anchors.fill : parent
            orientation: Qt.Horizontal
            Layers{
                id : lyrLeft
                width : parent.width / 2
                height : parent.height / 2
                panelSubIndex : 0
            }

            Layers{
                id : lyrRight
                width : parent.width / 2
                height : parent.height / 2
                panelSubIndex : 1
            }
        }
    }
    Layers{
        id : lyrDown
        width : parent.width
        height : parent.height / 2
        panelSubIndex : 2
    }


}