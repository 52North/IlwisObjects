import QtQuick 2.1
import QtQuick.Controls 1.1
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

	GroupBox {
    title: "Opening side"

		ColumnLayout {
			ExclusiveGroup { id: tabPositionGroup }
			RadioButton {
				id : b1
				text: "Left"
				exclusiveGroup: tabPositionGroup
			}
			RadioButton {
			    id : b2
				text: "Right"
				exclusiveGroup: tabPositionGroup
			}
			RadioButton {
			    id : b3
				text: "Other"
				checked : true
				exclusiveGroup: tabPositionGroup
			}
			Row {
				width : parent.width
				height : 30
				Button{
					width : 100
					height : 22
					text : "apply"
					onClicked : {
						var ids = mastercatalog.selectedIds();
						if ( ids != "") {
							var side = "other"
							if (b1.checked)
								side = "left"
							if ( b2.checked)
								side = "right"
							var parms = { 'side' : side, 'coverages' : ids}
							operation.execute(parms)
						}
					}
				}
			}
		}
	}
}