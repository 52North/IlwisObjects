import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../../../../Global.js" as Global
import "../../../../controls" as Controls
import "../../../../" as Base

Controls.DropableItem {
    id : dropContainer
    width : 80
    maxHeight: 80
    property var panelCallBack
    Rectangle {

        anchors.fill: parent
        color : "white"
        border.width: 1
        border.color: Global.edgecolor
        radius: 2
        ExclusiveGroup { id: layoutTypes }
        Grid {
            id : layouts
            columns : 2
            spacing : 2
            y : 4
            x : 4
            Controls.ToolButton { exclusiveGroup : layoutTypes; checkable : true; checked : true; width : 35; height : 35; iconH : 30; iconW : 30; iconSource : "../../../../images/mapanels301.png"; callBack : panelCallBack;type :"1"}
            Controls.ToolButton { exclusiveGroup : layoutTypes; checkable : true; width : 35; height : 35; iconH : 30; iconW : 30; iconSource : "../../../../images/mapanels30v2.png"; callBack : panelCallBack;type:"2 vertical"}
            //Controls.ToolButton { exclusiveGroup : layoutTypes; checkable : true; width : 35; height : 35; iconH : 30; iconW : 30; iconSource : "../../images/mapanels3021.png"; callBack : panelCallBack;type:"3: 2 by 1"}
            Controls.ToolButton { exclusiveGroup : layoutTypes; checkable : true; width : 35; height : 35; iconH : 30; iconW : 30; iconSource : "../../../../images/mapanels3021v.png"; callBack : panelCallBack;type:"3: 2 by 1 v"}
            Controls.ToolButton { exclusiveGroup : layoutTypes; checkable : true; width : 35; height : 35; iconH : 30; iconW : 30; iconSource : "../../../../images/mapanels304.png"; callBack : panelCallBack;type:"4"}
        }
     }
}
