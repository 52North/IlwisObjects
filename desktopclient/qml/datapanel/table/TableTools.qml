import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import OperationCatalogModel 1.0
import "../../controls" as Controls

ToolBar {
    id : tableTools
    width : parent.width
    height : 45

    property int buttonSize : 30

    Row {
        anchors.verticalCenter: parent.verticalCenter

        width : childrenRect.width
        height : buttonSize
        spacing : 3
        id : editingtools

        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("save.png")
            tooltip: qsTr("Saves the table in the current active catalog")

            onClicked: {
			console.log("Saving table");
                var url = mastercatalog.currentCatalog.url;
                if ( url.indexOf("file://") !== 0) {
                    url = table.url;
                    if(url.indexOf("file://") !== 0)
                        return;
                }
                table.store("","");
            }
        }
        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("saveas.png")
            tooltip: qsTr("Copies the table in the current active catalog under a different name")
            onClicked: {
			console.log("Table save as")
//                workflowView.storeRangeDefinitions()
//                workarea.dropSaveBox(x)
            }
        }

       Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("plus20.png")
            tooltip: qsTr("Adds a new record to the end of the records")
            onClicked: {
                table.addRecord(1)
            }
            visible : table ? !table.fixedRecordCount : false
            enabled : visible
        }

       Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("minus20.png")
            tooltip: qsTr("deletes the selected record/row")
            visible : table ? !table.fixedRecordCount : false
            enabled : visible
            onClicked: {
                table.removeRecord(tableView.currentRow)
            }
        }

       Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("insert20.png")
            tooltip: qsTr("inserts a record above the selected record")
            visible : table ? !table.fixedRecordCount : false
            enabled : visible
            onClicked: {
                table.insertRecord(tableView.currentRow)
            }
        }
    }

    function dropCondition(yesno){
//       newcondition.checked = yesno
    }

    function dropLoop(yesno){
       //loop.checked = yesno
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }

	Component.onCompleted: {
	}

}
