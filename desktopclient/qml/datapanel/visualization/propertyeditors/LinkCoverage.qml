import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import CoverageLayerModel 1.0
import "../../../Controls" as Controls
import "../../.." as Base
import "../../../Global.js" as Global

Item {
    width : Math.min(300,parent ? parent.width : 300)
    height : Global.rowHeight + 20
    property var editor

    Column {
        id : itemColumn
        width : parent.width - 5
        height : parent.height
        x : 5
        property var currentModelId : -1
        property bool needsUpdate : models.needsUpdate
        spacing : 3

        onNeedsUpdateChanged : {
            targetList.itemModel = models.modelList(editor.coverage().ilwisId, "all")
        }
      Text{
          width : parent.width
          height : 16
          text : qsTr("Target objects")
          font.bold : true
      }

        Controls.ComboxLabelPair {
            id : targetList
            labelText : ""
            labelWidth : 0
            role : 'name'
            width : parent.width - 5
            height : 20
  
            onCurrentIndexChanged : {
                if ( currentIndex >= 0){
                    itemColumn.currentModelId = targetList.itemModel[currentIndex].modelid
                }
            }
        }

      Text{
          width : parent.width
          height : 16
          text : qsTr("Target Linked property")
          font.bold : true
      }
      Controls.ComboxLabelPair {
        id : propertyList
            labelText :  ""
            labelWidth : 0
            role : 'name'
            width : parent.width - 5
            height : 20
            itemModel : itemColumn.currentModelId != -1 ? models.linkedProperties(itemColumn.currentModelId) : null

        }

        CheckBox {
            width : 120
            height : 20
            text : qsTr("Bi directional")
            style : Base.CheckBoxStyle1{}
        }

        Item {
            height : 40
            width : 100
            Button{
                    text : qsTr("Link");
                    height : 20
                    width : 100
                    anchors.verticalCenter : parent.verticalCenter
                    onClicked: {
                        var parameters = {sourceid: editor.coverage().modelId(),targetid : itemColumn.currentModelId, linktype : propertyList.itemModel[propertyList.currentIndex].method}
                        editor.setLink(parameters)
                    }
                }
           }
       }
}