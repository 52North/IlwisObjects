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
  spacing : 4
  property var editor
  property bool readOnly : false
 
    TableView {
        id : tableview
        width : parent.width
        height : parent.height - 65
        selectionMode : SelectionMode.SingleSelection
        property var doUpdate : true
        model : editor ? editor.controlPoints : null
        alternatingRowColors : false

        property bool ignore : false
        

        function setSelection(row){
            if ( row != editor.selectedRow && !ignore){
                tableview.selection.clear()
            
                tableview.selection.select(row)
                tableview.ignore = true
                editor.selectedRow = row
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
					enabled : !readOnly
                }
        }
        TableViewColumn{
            id : labelColumn
            width : 44
            title : qsTr("Label")
            role : "label"
            delegate :  
                TextField {
                    text: styleData.value
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter
                    textColor : editor.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                    onEditingFinished : {
                        editor.controlPointLabel(styleData.row,text)
                    }
                }
        }
        TableViewColumn{
            id : xColumn
            width : 73
            title : qsTr("X")
            role : "x"
            delegate :  
                TextField {
                    text: styleData.value.toFixed(editor.decimalsCrds)
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter
                    textColor : editor.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                    onEditingFinished : {
                        editor.changeTiePointCoord(styleData.row, text, editor.tiePointY(styleData.row))
                    }
                }
        }

        TableViewColumn{
            id : yColumn
            width : 73
            role : "y"
            title : qsTr("Y")
            delegate : 
                TextField {
                    text: styleData.value.toFixed(editor.decimalsCrds)
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter
                    textColor : editor.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                    onEditingFinished : {
                        editor.changeTiePointCoord(styleData.row, editor.tiePointX(styleData.row), text)
                    }
             }
        }

        TableViewColumn{
            id : cColumn
            width : 40
            title : qsTr("Column")
            role : "column"
            delegate : 
                TextField {
                    text: styleData.value.toFixed(editor.subPixelPrecision ? 1 : 0)
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter

                    textColor : editor.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                    onEditingFinished : {
                        editor.changeTiePointPixel(styleData.row, text, editor.tiePointRow(styleData.row), true)
                    }
             }
        }

        TableViewColumn{
            id : rColumn
            width : 40
            title : qsTr("Row")
            role : "row"
            delegate : 
                TextField {
                    text: styleData.value.toFixed(editor.subPixelPrecision ? 1 : 0)
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter

                    textColor : editor.selectedRow == styleData.row  ? "blue" : "black"

                    onActiveFocusChanged : {
                        tableview.setSelection(styleData.row )
                    }
                    onEditingFinished : {
                        editor.changeTiePointPixel(styleData.row, editor.tiePointColumn(styleData.row), text, true)
                    }
             }
        }
        TableViewColumn{
            id : dcColumn
            width : 45
            title : qsTr("dColumn")
            role : "columnError"
            delegate : 
                Text {
                    text: styleData.value != -1000000 ? styleData.value.toFixed(2) : '?'
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter

                    color : editor.selectedRow == styleData.row  ? "blue" : "black"
             }
        }
        TableViewColumn{
            id : rcColumn
            width : 45
            title : qsTr("dRow")
            role : "rowError"
           delegate : 
                Text {
                    text: styleData.value != -1000000 ? styleData.value.toFixed(2) : '?'
                    height : 20
					enabled : !readOnly
                    verticalAlignment:Text.AlignVCenter
                    color : editor.selectedRow == styleData.row  ? "blue" : "black"

             }
        }

           rowDelegate: Rectangle {
                id : rowdelegate
                height : 24
                color : editor.selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }
            itemDelegate: Rectangle {
                height : 24
                color : editor.selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }

        Component.onCompleted : {
			var url = { 'url' :georefUrl()}
            editor = objectcreator.createControlPointsList(url, tableview)
         }
  
    }
    TextArea {
        id : errors
        width : parent.width 
        height : 40
        text : editor.errors
    }
    Row {
        anchors.right : parent.right
        width : 190
        height : 25
        spacing : 4
        visible : allDataSet()
         Button {
             width : 90
             height : 22
             text : qsTr("Delete tiepoint")
             onClicked : {
                 var oldRow = editor.selectedRow
                 editor.removeTiepoint(editor.selectedRow)
                 editor.selectedRow = oldRow 
                 tableview.currentRow = editor.selectedRow
             }
         }
         Button {
             width : 90
             height : 22
             text : qsTr("Add Add tiepoint")
             onClicked : {
                 editor.addTiepoint()
                 editor.selectedRow = editor.controlPoints.length - 1
                 tableview.currentRow = editor.selectedRow
             }
         }
    }

       function handleMouseClick(mx,my){
        if ( editor.selectedRow >= 0){
       
           editor.changeTiePointPixel(editor.selectedRow, mx, my, false)
        }
    }
}