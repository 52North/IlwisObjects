import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

SplitView {
    orientation: Qt.Vertical
    
    function activePanel(){
        if ( layerview.activeSubPanel == 0)
            return lyrLeftTop
        if ( layerview.activeSubPanel == 1)
            return lyrRightTop
        if ( layerview.activeSubPanel == 2)
            return lyrLeftBottom
        return lyrRightBottom
    }

    function assignLayerManagers() {
        lyrLeftTop.assignLayerManager(layerview.layermanagers[0])        
        lyrRightTop.assignLayerManager(layerview.layermanagers[1])
        
        lyrLeftBottom.assignLayerManager(layerview.layermanagers[2])        
        lyrRightBottom.assignLayerManager(layerview.layermanagers[3])           
    }

    SplitView {
        width : parent.width
        height : parent.height / 2
        orientation: Qt.Horizontal

        Layers{
            id : lyrLeftTop
            width : parent.width / 2
            height : parent.height
            panelSubIndex : 0
        }

        Layers{
            id : lyrRightTop
            width : parent.width / 2
            height : parent.height
            panelSubIndex : 1
        }
    }
    SplitView {
        width : parent.width
        height : parent.height / 2
        orientation: Qt.Horizontal
        Layers{
            id : lyrLeftBottom
            width : parent.width / 2
            height : parent.height
            panelSubIndex : 2
        }

        Layers{
            id : lyrRightBottom
            width : parent.width / 2
            height : parent.height
            panelSubIndex : 3
        }
    }


}