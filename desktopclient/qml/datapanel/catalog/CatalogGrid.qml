import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0

import "../../Global.js" as Global

FocusScope {
    anchors.fill: parent
    property color backgroundColor: uicontext.genericBGColor
    Rectangle {
    id : cont
        anchors.fill :parent
        color : mastercatalog.metadataEditMode ? "#f2f2f2" : uicontext.catalogBGColor


        ScrollView {
            id : catalogGrid
            anchors.fill : parent
            anchors.margins: 2

            GridView {
                id : itemgrid

                Loader {
                    id : floatingProps
                    width :270
                    height : 150
                    z : 100
                }

                model : currentCatalog ? currentCatalog.resources : null
                width : parent.width
                height : parent.height

                delegate: CatalogGridDelegate{}
                cellWidth: 170
                cellHeight: 22
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
            }
        }
    }
}



