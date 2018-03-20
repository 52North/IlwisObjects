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

    clip:true


    Rectangle {
        id : container
        height: 590
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5

        property var columnArray : []

        Column {
            width : parent.width - 3
            height : 320
            spacing : 4
            x : 3
            y : 4
            Controls.TextEditLabelPair{
                id : namefield
                labelText: qsTr("Name")
                labelWidth: 100
                height : 20
                width : parent.width - 3
            }

            TableView {
                id : columntable
                width : parent.width - 3
                height : 150
                selectionMode : SelectionMode.ExtendedSelection

                TableViewColumn{
                    title : qsTr("Column Name");
                    role : "name"
                    width : 150
                    delegate: Component{
                        Text {
                            text: styleData.value
                            verticalAlignment:Text.AlignVCenter
                            font.pixelSize: 10
                            elide: Text.ElideMiddle
                            x : 4
                        }
                    }
                }
                TableViewColumn{
                    title : qsTr("Domain");
                    role : "domain"
                    width : 230
                    delegate: Component{
                        Text {
                            text: styleData.value
                            verticalAlignment:Text.AlignVCenter
                            font.pixelSize: 10
                            elide: Text.ElideMiddle
                        }
                    }
                }

               rowDelegate: Rectangle {
                    id : rowdelegate
                    height : 15
                    color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
                }

            }

            ColumnCreator{
            }

            ApplyCreateButtons {
                width : parent.width
                height : 60
                id : applyButton
                createObject: dropItem.apply

            }
        }

  

    }
    function addColumn(item,numericColumn){
        if ( numericColumn){
        }else {
            if ( item.name === "")
                return
            container.columnArray.push({name : item.name, domain : item.domain })
            columntable.model = container.columnArray
        }

    }

    function apply(overwrite) {
       var url = applyButton.currentCatalogCorrectUrl() + "/"+ namefield.content
    
    
       var columnscom = ""
       for(var i=0; i < container.columnArray.length; ++i ){
        
            if ( columnscom != "")
                columnscom += "|"
             columnscom += container.columnArray[i].name
             columnscom += "|" 
             columnscom += container.columnArray[i].domain
       }
        var createInfo = {type : "table", name : url, columns : columnscom}
       objectcreator.createObject(createInfo)
       return true
    }
}




