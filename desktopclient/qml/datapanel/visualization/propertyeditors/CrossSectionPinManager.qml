import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CrossSectionPin 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Item {
	anchors.fill : parent
	anchors.leftMargin : 4
	property var selectedRow : -1


	Controls.Busy {
		id : busy

	}

	Column { 
		anchors.fill : parent

		Item {
			width : 120
			height : 20
			Text {
				anchors.fill : parent
				visible : !editor.hasData
				text : qsTr("No data soource yet set")
				color : "red"
			}
			CheckBox {
				width : parent.width
				height : parent.height
				visible : editor.hasData
				checked : editor.contineousMode()
				style : Base.CheckBoxStyle1{}
				text : qsTr(" Continuous Mode")

				onCheckedChanged : {
				    if(editor.hasData) {
						editor.contineousMode(checked)
						if (checked){
							crosssectiontool.contineousPin = editor.addContineousPin()
						}else {
							var columnName = editor.pinDataColumn(crosssectiontool.contineousPin + 1)
							editor.deletePin( crosssectiontool.contineousPin)
							var expr = "deletechartseries(" + modelid + "," + columnName + ")"
							layerview.activeLayerManager().addCommand(expr);
							crosssectiontool.contineousPin = -1

						}
					}

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
						id : label
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
							onEditingFinished : {
								//editor.pinlabel = label.text
								//var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(styleData.row + 1) + ")"
								//layerview.activeLayerManager().addCommand(expr);
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
							var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(styleData.row + 1) + ")"
							layerview.activeLayerManager().addCommand(expr);
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
							var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(styleData.row + 1) + ")"
							layerview.activeLayerManager().addCommand(expr);
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
							var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(styleData.row + 1) + ")"
							layerview.activeLayerManager().addCommand(expr);
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
							var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(styleData.row + 1) + ")"
							layerview.activeLayerManager().addCommand(expr);
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
			visible : crosssectiontool.contineousPin == -1 && editor.hasData


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
}