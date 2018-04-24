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
    property var chartType

    clip:true

    onDataTableChanged : {
        xaxisCB.itemModel = dataTable.attributes
        yaxisCB.itemModel = dataTable.attributes
        zaxisCB.itemModel = dataTable.attributes
    }

    Column {
        x : 5
        height : 500
        width : parent.width - 5
        id : fields
        spacing : 5
        Controls.TextEditLabelPair{
            id : nameedit
            labelText: qsTr("Name")
            labelWidth: 100
            width : parent.width - 3
        }
        Controls.TextEditLabelPair{
            id : tableedit
            labelText: qsTr("Table")
            labelWidth: 100
            width : parent.width - 3
            keys : ['table']

            onContentChanged : {
                if ( ilwisobjectid){
                    dataTable = mastercatalog.id2object(ilwisobjectid, fields)
                }else
                    dataTable = null
            }
        }

        Controls.ComboxLabelPair {
            id : charttype
            labelText : qsTr("Chart type")
            labelWidth : 100
            width : parent.width - 5
            height : 20
            //role : "attributename"
            itemModel : ["Line chart", "Spline chart","Bar chart","Pie chart", "Points", "Polar chart", "3D Line chart", "3D Spline chart","3D barchart" ]

        }

        Controls.ComboxLabelPair {
            id : xaxisCB
            labelText : qsTr("X axis")
            labelWidth : 100
            width : parent.width - 5
            height : 20
            role : "attributename"

        }

        Controls.ComboxLabelPair {
            id : yaxisCB
            labelText : qsTr("Y axis")
            labelWidth : 100
            width : parent.width - 5
            height : 20
            role : "attributename"

        }
        Controls.ComboxLabelPair {
            id : zaxisCB
            labelText : qsTr("Z axis")
            labelWidth : 100
            width : parent.width - 5
            height : 20
            role : "attributename"
            visible : charttype.comboText.indexOf("3D") == 0 || charttype.comboText.indexOf("3D") == 0 || charttype.comboText.indexOf("3D") == 0

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
            var turl = tableedit.comboText
            if ( turl != "" && nameedit.content != ""){
            var createInfo = {type : "chart", url : turl, name : nameedit.content , xaxis : xaxisCB.comboText, yaxis : yaxisCB.comboText, zaxis : zaxisCB.comboText}
            objectcreator.createObject(createInfo)
            var filter = "itemid=" + dataTable.id
            bigthing.newCatalog(filter, "chart", "","other")
            return true
          }
       }
       return false
    }
}