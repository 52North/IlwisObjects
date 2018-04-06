import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
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

/*        Loader {
            id : butLoader
            source : workflowOnly ? "" : "../GenericTools.qml"
            height : buttonSize
            width : workflowOnly ? 0 : 60
        }*/

        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("save.png")
            tooltip: qsTr("Saves the table in the current active catalog")

            onClicked: {
			console.log("Saving table");
//                workflowView.storeRangeDefinitions()
                var url = mastercatalog.currentCatalog.url;
				console.log("master="+url);
				console.log("table="+table)
				console.log("table_url="+table.url);
//				console.log("model="+model.rawUrl);
                if ( url.indexOf("file://") !== 0) {
                    url = table.url;
				console.log("table="+url);
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
