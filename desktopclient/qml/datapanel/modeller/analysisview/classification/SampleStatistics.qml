import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

TableView {
	id : items

	TableViewColumn{
		title : qsTr("Item name")
		role : "name"
		width : 80
		delegate: Component{
			Text {
				text: styleData.value
				verticalAlignment:Text.AlignVCenter
				font.pixelSize: 12
				elide: Text.ElideMiddle
			}
		}
	}
	TableViewColumn{
		title : qsTr("Mean")
		role : "mean"
		width : 60

		delegate: Component{
			Text {
				text: styleData.value
				verticalAlignment:Text.AlignVCenter
				font.pixelSize: 12
				elide: Text.ElideMiddle
			}
		}
	}
	TableViewColumn{
		title : qsTr("STDEV")
		role : "stdev"
		width : 60
		delegate: Component{
			Text {
				text: styleData.value
				verticalAlignment:Text.AlignVCenter
				font.pixelSize: 12
				elide: Text.ElideMiddle
			}
		}
	}
	TableViewColumn{
		title : qsTr("Predom")
		role : "predominant"
		width : 60
		delegate: Component{
			Text {
				text: styleData.value
				verticalAlignment:Text.AlignVCenter
				font.pixelSize: 12
				elide: Text.ElideMiddle
			}
		}
	}
	TableViewColumn{
		title : qsTr("Count")
		role : "count"
		width : 60
		delegate: Component{
			Text {
				text: styleData.value
				verticalAlignment:Text.AlignVCenter
				font.pixelSize: 12
				elide: Text.ElideMiddle 
			}
		}
	}
	rowDelegate: Rectangle {
		id : rowdelegate
		height : 20
		color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
	}
}