import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CrossSectionPin 1.0
import "../../../Global.js" as Global
import "../../.." as Base

Column { 
    anchors.fill : parent
    anchors.leftMargin : 4
    property var selectedRow : -1
    CheckBox {
        width : 120
        height : 20
        checked : editor.contineousMode()
        style : Base.CheckBoxStyle1{}
        text : qsTr(" Continuous Mode")

        onCheckedChanged : {
            editor.contineousMode(checked)
            if (checked){
                crosssectiontool.contineousPin = editor.addContineousPin()
            }else {
                editor.deletePin( crosssectiontool.contineousPin)
                crosssectiontool.contineousPin = -1
            }

        }
    }
    TableView {
            id : tableview
            width : parent.width
            height : parent.height - 60
            selectionMode : SelectionMode.SingleSelection
            alternatingRowColors : false
            model :  editor ? editor.pins : 0

            TableViewColumn{
                id : dummyColumn
                width : 10
            }
            TableViewColumn{
                id : nameColumn
                role : "pinlabel"
                title : qsTr("Label")
                width : 80
                delegate :  
                    TextField {
                        text: styleData.value
                        height : 20
                        verticalAlignment:Text.AlignVCenter

                        onActiveFocusChanged : {
                            tableview.selection.clear()
                            if ( activeFocus){
                                tableview.selection.select(styleData.row)
                                selectedRow  = styleData.row 
                            }    
                        }
                    }
            }
           TableViewColumn{
                id : columnColumn
                role : "pincolumn"
                title : qsTr("Column")
                width : 43
                delegate :  TextField {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    validator : IntValidator{bottom : 0; top : editor.maxColumn;}

                    onActiveFocusChanged : {
                            tableview.selection.clear()
                            if ( activeFocus){
                                tableview.selection.select(styleData.row)
                                selectedRow  = styleData.row 
                            }    
                    }
                    onEditingFinished : {
                        editor.changeCoords(styleData.row, text, editor.pinRow(styleData.row), false)
                    }
                }
            }
            TableViewColumn{
                id : rowColumn
                role : "pinrow"
                title : qsTr("Row")
                width : 43
                delegate :  TextField {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    validator : IntValidator{bottom : 0; top : editor.maxRow;}
                    onActiveFocusChanged : {
                            tableview.selection.clear()
                            if ( activeFocus){
                                tableview.selection.select(styleData.row)
                                selectedRow  = styleData.row 
                            }    
                    }
                    onEditingFinished : {
                        editor.changeCoords(styleData.row, editor.pinColumn(styleData.row), text, false)
                    }
                }
            }
            TableViewColumn{
                id : xColumn
                role : "pinx"
                title : qsTr("X")
                width : 75
                delegate :  TextField {
                    text: styleData.value.toFixed(editor.decimalsCrds)
                    verticalAlignment:Text.AlignVCenter
                    //validator : DoubleValidator{bottom : editor.minX; top : editor.maxX;}
                    cursorPosition : 0
                    onActiveFocusChanged : {
                        tableview.selection.clear()
                        if ( activeFocus){
                            tableview.selection.select(styleData.row)
                            selectedRow  = styleData.row 
                        }    
                    }
                    onEditingFinished : {
                        editor.changePixel(styleData.row, text, editor.pinY(styleData.row))
                    }
                }
            }
            TableViewColumn{
                id : yColumn
                role : "piny"
                title : qsTr("Y")
                width : 75
                delegate :  TextField {
                    text: styleData.value.toFixed(editor.decimalsCrds)
                    verticalAlignment:Text.AlignVCenter
                    //validator : DoubleValidator{bottom : editor.minY; top : editor.maxY;}
                    cursorPosition : 0
                    onActiveFocusChanged : {
                            tableview.selection.clear()
                            if ( activeFocus){
                                tableview.selection.select(styleData.row)
                                selectedRow  = styleData.row 
                            }    
                    }
                    onEditingFinished : {
                        editor.changePixel(styleData.row, editor.pinX(styleData.row), text)
                    }
                }
            }
            rowDelegate: Rectangle {
                id : rowdelegate
                height : 24
                color : selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }
            itemDelegate: Rectangle {
                height : 24
                color : selectedRow == styleData.row ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
            }
     }
     Row {
        anchors.right : parent.right
        width : 260
        height : 25
        spacing : 4
        visible : crosssectiontool.contineousPin == -1


         Button {
             width : 80
             height : 22
             text : qsTr("Delete pin")
             onClicked : {
                 editor.deletePin(selectedRow)
             }
         }
         Button {
             width : 80
             height : 22
             text : qsTr("Add pin")
             onClicked : {
                 editor.addPin()
                 selectedRow = editor.pins.length - 1
                 tableview.currentRow = selectedRow
             }
         }
		 Button {
             width : 80
             height : 22
             text : qsTr("Show Chart")
			 enabled : editor.pinCount > 0
             onClicked : {
				var mdl = models.model(modelid)
			    if ( mdl == null && editor.pinCount > 0)
					reopenChart() 
             }
         }
     }
}