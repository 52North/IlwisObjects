import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import TabModel 1.0
import ModelRegistry 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Item {
    id : tablePane
    width: parent.width
    height : parent.height
    property bool canSeparate : true
    property string panelType : "tablepanel"
    property var createParameters : []
    property TabModel tabmodel

    property TableModel table

	function setTable(t){
		table = t
		tableView.model = null
		tableView.model = t

	}
    Component{
        id : column
        TableColumn{
        }
    }


	TableTools {
		id: tableToolbar
		z: 10
	}

    SplitView {
//        anchors.fill: parent
	    anchors.top:tableToolbar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
        orientation: Qt.Vertical
        height : parent.height - tableToolbar.height


        TableView {
            id : tableView
            width : parent.width
            height : parent.height - 270 - tableToolbar.height
            selectionMode : SelectionMode.ExtendedSelection
            property int currentSelection : table ? table.currentSelection : -1

            onCurrentSelectionChanged : {
                if ( currentSelection < rowCount && currentSelection >= 0){
                    selection.deselect(0, rowCount)
                    selection.select(currentSelection) 
                } else
                    selection.select(-1) 
            }

            onCurrentRowChanged : {
                if ( currentRow >= 0){
                    var parms = {sourceid : table.modelId(), records : currentRow}
                    table.linkMessage(parms)
                }
           }

            headerDelegate : ColumnHeader{}


            rowDelegate: Rectangle {
                id : rowdelegate
                height : 20
                color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? "#eee" : "#fff")
            }

            states: [
                State { name: "visible"

                    PropertyChanges {
                        target: tableView
                        height : parent.height - 270 - tableToolbar.height
                    }
                },
                State {
                    name : "invisible"
                    PropertyChanges {
                        target: tableView
                        height : parent.height  - 24 - tableToolbar.height
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]

        }
        ColumnManagement{
            id : columnManagement
            height : 270
        }
    }

    function defaultWidth(index){
        if (!table)
            return 60
        return table.defaultWidth(index)
    }

    function transfer(datapanel) {
        if ( datapanel.table)    {
            addDataSource(datapanel.table.url, "", "table")
        }
    }

    function addDataSource(filter, sourceUrl, sourceType){
        table = models.createTableModel(tablePane,sourceUrl, sourceType)

        if ( table){
            tableView.model = table
            columnManagement.setColumnModel1(table.columns)

            for(var i =0; i < table.columnCount; ++i){
                if ( i == 0)
                    tableView.addColumn(column.createObject(tableView,{"role" : "first", "title" : "nr", "width" : defaultWidth(i)}))
                else {
                    var roleName = table.roleName(i)
                    var dw = table.defaultWidth(i)
                    tableView.addColumn(column.createObject(tableView,{"role" : roleName, "title" : roleName, "width" : dw}))
                }
            }
            createParameters = [sourceUrl, filter, sourceType]
            return table.id
        }
    }
}

