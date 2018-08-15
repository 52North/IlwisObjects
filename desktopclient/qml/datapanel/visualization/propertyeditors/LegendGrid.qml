import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Column {
    function setColor(clr){
        if ( grid.currentIndex != -1){
            grid.model[grid.currentIndex].ecolor = clr
            var expr = "attributefillcolor(" + activeLayerManager().viewid +"," + editor.layerIndex + "," + editor.attributeName + "," + grid.model[grid.currentIndex].label + "," + clr +")"
            activeLayerManager().addCommand(expr)
        }
    }

    function setOpacity(index, opacity){
        if ( grid.currentIndex != -1){
            var expr = "attributeopacity(" + activeLayerManager().viewid +"," + editor.layerIndex + "," + editor.attributeName + "," + grid.model[index].label + "," + opacity +")"
            activeLayerManager().addCommand(expr)
        }
    }


    Controls.ColorPicker2{
        id : rprName
        width : Math.min(250, parent.width * 0.8)
        x : 14
        y : 4
        visible : grid.currentIndex != -1

        onSelectedColorChanged: {
            legendGrid.setColor(selectedColor)
        }
        z : 100
    }

    ScrollView {
        id : scroller
        width : parent.width  - 5
        height : parent.height - 20
        z : 0

        x : 5

        GridView{
            id : grid
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.leftMargin : 2
            property int maxChar : 15

            model : editor.representationElements
            onModelChanged: {
                var maxl = 0
                for(var i = 0; i < model.length; ++i)    {
                    maxl = Math.max(maxl, model[i].label.length)
                }
                maxChar = maxl
            }

            cellWidth: 85 + Math.max(30, maxChar * 6)
            cellHeight: 23
            flow: GridView.FlowTopToBottom
            highlight: Rectangle { color: Global.selectedColor; radius: 2 }
            delegate: Component{
                Item{
                    width :  GridView.view.cellWidth
                    height :  GridView.view.cellHeight
                    Row{
                        width : parent.width
                        height : 20
                        anchors.verticalCenter: parent.verticalCenter
                        spacing : 3
                        CheckBox {
                            id : cb
                            width : 20
                            height : 20
                            checked : eopacity == 1
                            style: Base.CheckBoxStyle1{}

                            onClicked : {
                                grid.currentIndex = index
                                setOpacity(index, checked ? 1 : 0)
                            }
                        }
                        Rectangle{
                            id : colorrect
                            width : 30
                            height : 20
                            color : ecolor
                            border.width: 1
                            border.color : "grey"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text{
                            height : 50
                            width : parent.width - colorrect.width - 10
                            text : label
                            y : 4
                            clip : true
                            elide: Text.ElideMiddle
                        }

                    }
                    MouseArea{
                        x : 20
                        width : parent.width - 20
                        height : parent.height
                        onClicked: {
                            var isChanged = index != grid.currentIndex
                            grid.currentIndex = index
                            if (legend.state == "visible" && !isChanged){
                                legend.state = "invisible"
                                grid.currentIndex = -1
                            }
                            else{
                                legend.state = "visible"
                                rprName.initialColor = ecolor
                            }
                        }
                    }
                }
            }
            Component.onCompleted: {
                currentIndex = -1
            }
        }
    }
    

}

