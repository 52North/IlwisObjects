import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import QtQuick.Controls 2.3 as QC2
import OperationModel 1.0
import MasterCatalogModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Column {
	spacing : 4
	Controls.BoundingBox {
		id : boundingbox
	}
	Row {
		spacing : 4
		Controls.PushButton {
			width : 100
			height : 20
			text : qsTr("apply")
			onClicked : {
				var bb = "boundingbox(" + boundingbox.content + ")"
				addRule({ rule : bb, connector : "with"})
				closeMe()
			}
		}
		Controls.PushButton {
				width : 100
				height : 20
				text : qsTr("Cancel")

				onClicked : {
					closeMe()
				}
		}
	}
}