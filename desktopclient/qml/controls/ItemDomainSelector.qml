import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../Global.js" as Global

GridView{
    property var domainModel

    id : grid
    height :65
    cellWidth: 130
    cellHeight: 15
    width : 260
    property var checks : []
    model : domainModel ? domainModel.domainitems : null
    function selectedItems() {
        var items = []
        for(var i=0; i <  model.length; ++i){
            var item = {name : model[i].name, value : checks[i]} 
            items.push(item)
        }
       
        return items
    }

    onModelChanged : {
        if ( model) {
            for(var i=0; i <  model.length; ++i)
                checks.push(true)
        }
    }

    clip : true
    delegate : Row  {
        CheckBox {
            id : checkIncluded
            width : 20
            height : 15
            checked : true

            onCheckedChanged : {
                checks[index] = checked
            }
        }
        Text {
            text : name
            height : 15
            width : grid.cellWidth - checkIncluded.width - 2
            font.pointSize: 8
            font.italic: true
            elide : Text.ElideMiddle
        }
    }
}
