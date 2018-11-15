import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import IlwisObjectCreatorModel 1.0

import "../.." as Base
import "../../workbench" as Workbench
import "../../Global.js" as Global
import "../../controls" as Controls

Controls.DropableItem{
    width : parent.width
    height : 0
    clip:true

    property var keylist : []
    property bool isNew : true

    function currentCatalogCorrectUrl(){ // must be a file location
        return ""
    }

    Item {
        x : 4
        width : parent.width - 5
        height : 750
        Rectangle {
            anchors.fill: parent
			Button {
				text : qsTr("Open Python Console")
				width : parent.width
				height : 22
				onClicked : {
					var id = uicontext.consoleScriptId()
                    bigthing.newCatalog("itemid=" + id, "script", "ilwis://internalcatalog/consolescript.py", "other")
				}
			}
		}
   

    }
}

