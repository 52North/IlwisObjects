import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

SplitView {
    orientation: Qt.Horizontal
    
    function activePanel(){
        if ( layerview.activeSubPanel == 0) 
            return lyrLeft
        if ( layerview.activeSubPanel == 1)
            return lyrMiddle
        return lyrRight
    }

    function assignLayerManagers() {
        lyrLeft.assignLayerManager(layerview.layermanagers[0])        
        lyrMiddle.assignLayerManager(layerview.layermanagers[1])  
        lyrRight.assignLayerManager(layerview.layermanagers[2]) 
    }

    Layers{
        id : lyrLeft
        width : parent.width / 3
        height : parent.height
        panelSubIndex : 0
    }

    Layers{
        id : lyrMiddle
        width : parent.width / 3
        height : parent.height
        panelSubIndex : 1
    }

    Layers{
        id : lyrRight
        width : parent.width / 3
        height : parent.height
        panelSubIndex : 2
    }


}