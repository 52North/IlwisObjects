import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

Rectangle {
    anchors.fill: parent
    anchors.topMargin: 3
    color : catalogSplit.backgroundCatalogColor
	property string iconName : "../images/display_options"

	  SplitView {
		width : parent.width - 5
        height : parent.height
        y : 2
		id : catoperations

		CatalogOperationList{
            id : operationColumn
            height : parent.height
            width : 300
            anchors.left: parent.left
        }

		CatalogOperationEditor{
            height : parent.height - 3
            width : catoperations.width - operationColumn.width
            id : editorColumn
        }
	  }
}