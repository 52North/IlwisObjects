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
        return lyrRight
    }

    function assignLayerManagers() {
        lyrLeft.assignLayerManager(layerview.layermanagers[0])        
        lyrRight.assignLayerManager(layerview.layermanagers[1])  
    }

    Layers{
        id : lyrLeft
        width : parent.width / 2
        height : parent.height
        panelSubIndex : 0
    }

    Layers{
        id : lyrRight
        width : parent.width / 2
        height : parent.height
        panelSubIndex : 1
    }


}