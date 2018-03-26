import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0
    property var dataTable

    clip:true


    Column {
        height : 500
        width : parent.width - 5
        id : fields
        Controls.TextEditLabelPair{
            id : nameedit
            labelText: qsTr("Name")
            labelWidth: 100
            width : parent.width
        }
        Controls.TextEditLabelPair{
        id : columndomain
            labelText: qsTr("Table")
            labelWidth: 100
            width : parent.width
            keys : ['table']

            onContentChanged : {
                if ( ilwisobjectid){
                    dataTable = mastercatalog.id2object(ilwisobjectid, fields)
                }else
                    dataTable = null
            }
        }
        ApplyCreateButtons {
            width : parent.width
            height : 60
            id : applyButton
            createObject: dropItem.apply

        }
    }



    function apply(overwrite) {

        if ( dataTable) {
          var dummy = "ilwis:///dummy.dummmy.dum"
          var createInfo = {type : "chart", url : dummy}
          objectcreator.createObject(createInfo)
          var filter = "itemid=" + dataTable.id
          bigthing.newCatalog(filter, "chart", "","other")
          return true
       }
       return false
    }
}