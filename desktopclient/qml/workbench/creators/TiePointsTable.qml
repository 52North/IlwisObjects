import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import ControlPointModel 1.0
import ControlPointsListModel 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
  width : parent.width
    height : 140

  //  Component {
     //   id : textstyle
 

    //}
    TableView {
        id : tableview
        width : parent.width
        height : 120
        selectionMode : SelectionMode.SingleSelection
        property var doUpdate : true
        property var tiePointList
        alternatingRowColors : false
        property int dummy : 0
        property var selectedRow : -1
        property bool ignore : false
        model : tableview.tiePointList ? tableview.tiePointList.controlPoints : null

        function setSelection(row){
            tableview.dummy = tableview.dummy + 1
            console.debug("before", tableview.dummy , ":", row, focus, activeFocus, tableview.selectedRow, tableview.ignore)
            if ( row != tableview.selectedRow && !ignore){
                tableview.selection.clear()
            
                tableview.selection.select(row)
                tableview.ignore = true
                tableview.selectedRow  = row
                console.debug("after", tableview.dummy , ":", row, tableview.selectedRow, tableview.ignore)
                return
            }
            ignore = false 
        }

        TableViewColumn{
            id : activeColumn
            width : 25
            role : "active"
            title : qsTr("Active")
            delegate : 
                CheckBox {
                    width : 18
                    height : 18
                    checked : styleData.value
                    style : Base.CheckBoxStyle1{}
                }
        }
        TableViewColumn{
            id : xColumn
            width : 75
            title : qsTr("X")
            role : "x"
            delegate :  
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter
                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                }
        }

        TableViewColumn{
            id : yColumn
            width : 75
            role : "y"
            title : qsTr("Y")
            delegate : 
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter
                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
             }
        }

        TableViewColumn{
            id : cColumn
            width : 45
            title : qsTr("Column")
            role : "column"
            delegate : 
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter

                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
             }
        }

        TableViewColumn{
            id : rColumn
            width : 45
            title : qsTr("Row")
            role : "row"
            delegate : 
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter

                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
             }
        }
        TableViewColumn{
            id : dcColumn
            width : 50
            title : qsTr("dColumn")
            role : "columnError"
            delegate : 
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter

                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
             }
        }
        TableViewColumn{
            id : rcColumn
            width : 50
            title : qsTr("dRow")
            role : "rowError"
           delegate : 
                TextField {
                    text: styleData.value
                    height : 20
                    verticalAlignment:Text.AlignVCenter

                    textColor : tableview.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
             }
        }

           rowDelegate: Rectangle {
                id : rowdelegate
                height : 24
                color : tableview.selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }
            itemDelegate: Rectangle {
                height : 24
                color : tableview.selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }

        Component.onCompleted : {
            tableview.tiePointList = objectcreator.createControlPointsList(tableview)
  
        }
  
    }
    Row {
        anchors.right : parent.right
        width : 190
        height : 25
        spacing : 4
         Button {
             width : 90
             height : 22
             text : qsTr("Delete tiepoint")
                   }
         Button {
             width : 90
             height : 22
             text : qsTr("Add Add tiepoint")
             onClicked : {
                 tableview.tiePointList.addTiepoint()
             }
         }
    }
}