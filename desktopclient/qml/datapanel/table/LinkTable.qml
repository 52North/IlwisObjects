import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import "../../Controls" as Controls

Column {
    property var operation
    property int columnIndex
    anchors.fill : parent
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
        width : 460
        height : 20
  
        onCurrentIndexChanged : {
            if ( currentIndex >= 0){
                currentModelId = targetList.itemModel[currentIndex].modelid
            }
        }
    }

  Controls.ComboxLabelPair {
    id : propertyList
        labelText :  qsTr("Linked Target property")
        labelWidth : 160
        role : 'name'
        width : 460
        height : 20
        itemModel : currentModelId != -1 ? models.linkedProperties(currentModelId) : null
    }

  Button{
            text : qsTr("Link");
            height : 20
            width : 100
            onClicked: {
                var paramaters = {sourceid: table.modelId(),targetid : currentModelId, linktype : propertyList.itemModel[propertyList.currentIndex].method}
                operation.execute(paramaters)
            }
        }
}