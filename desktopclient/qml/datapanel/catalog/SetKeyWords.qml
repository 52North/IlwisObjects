import QtQuick 2.1
import QtQuick.Controls 1.4 as QC1
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Rectangle {
    width: parent.width - 5
    height: parent ? parent.height - 10 : 0
    property var operation

	Column {
		width : parent.width
		height : 60
		spacing : 4
		Controls.TextEditLabelPair {
		    id : keys
			labelText : "Keywords"
			labelWidth : 100
			width : Math.min(450, parent.width)

		}
		QC1.Button{
			width : 100
			height : 22
			text : "apply"
			onClicked : {
				var ids = mastercatalog.selectedIds();
				var parms = { 'keywords' : keys.content, 'objects' : ids}
				operation.execute(parms)
			}
		}
	}
}