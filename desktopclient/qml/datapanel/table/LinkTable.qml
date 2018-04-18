import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import "../../Controls" as Controls
import "../.." as Base

Item {
    anchors.fill : parent
    property var operation
Column {
    id : itemColumn

    property int columnIndex
    width : parent.width - 5
    height : parent.height
    x : 5
    property var currentModelId : -1
    property bool needsUpdate : models.needsUpdate
    spacing : 4

    onNeedsUpdateChanged : {
        targetList.itemModel = models.modelList(table.modelId(), "all")
    }

    Controls.ComboxLabelPair {
        id : targetList
        labelText :  qsTr("Target objects")
        labelWidth : 160
        role : 'name'
        width : 400
        height : 20
  
        onCurrentIndexChanged : {
            if ( currentIndex >= 0){
                itemColumn.currentModelId = targetList.itemModel[currentIndex].modelid
            }
        }
    }

  Controls.ComboxLabelPair {
    id : propertyList
        labelText :  qsTr("Linked Target property")
        labelWidth : 160
        role : 'name'
        width : 400
        height : 20
        itemModel : itemColumn.currentModelId != -1 ? models.linkedProperties(itemColumn.currentModelId) : null

    }

    CheckBox {
        width : 120
        height : 20
        text : qsTr("Bi directional")
        style : Base.CheckBoxStyle1{}
    }

    Button{
            text : qsTr("Link");
            height : 20
            width : 100
            anchors.leftMargin : 5
            onClicked: {
                var paramaters = {sourceid: table.modelId(),targetid : itemColumn.currentModelId, linktype : propertyList.itemModel[propertyList.currentIndex].method}
                operation.execute(paramaters)
            }
        }
   }
}