import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    width : 27
    color : Global.alternatecolor1

    Action {
        id : entiremapClicked
        onTriggered : {
            if ( metatdata.manager){
                layerview.entireMap()
                //metatdata.manager.wholeMap() // this performs entireMap() on the overview window; obsolete
            }
        }
    }
    Action {
        id : zoomClicked
        onTriggered : {
            if ( metatdata.manager){
                zoominButton1.checked = !zoominButton1.checked
                metatdata.manager.zoomInMode = zoominButton1.checked
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter

        Controls.MapExtentButton{
            id : entiremapButton1
            icon : enabled ? "full_green.svg" : "full_grey.svg"
            checkable: true
            checked: false
            pushed : pressed
            action : entiremapClicked
        }
        Controls.MapExtentButton{
            id : zoominButton1
            icon : enabled ? "zoomin_green.svg" : "zoomin_grey.svg"
            checkable: true
            checked: false
            pushed : checked
            action : zoomClicked
        }
    }
}
