import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

SplitView {
    orientation: Qt.Vertical
    
    function activePanel(){
        if ( layerview.activeSubPanel == 0)
            return lyrUp
        return lyrDown
    }

    function assignLayerManagers() {
        lyrUp.assignLayerManager(layerview.layermanagers[0])        
        lyrDown.assignLayerManager(layerview.layermanagers[1])
    }

    Layers{
        id : lyrUp
        height : parent.height / 2
        width : parent.width
        panelSubIndex : 0
    }

    Layers{
        id : lyrDown
        height : parent.height / 2
        width : parent.width
        panelSubIndex : 1
    }


}