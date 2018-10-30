import QtQuick 2.2
import QtQuick.Controls 1.5
import QtQml.Models 2.2
import FileSystem 1.0

Item {
	width : 400
	height : 250
	property alias currentUrl : folders.currentUrl

	FileSystem {
		id : folders
		root : ""
		filter : "dirs|nodots|drives"
	}

	TreeView {
		anchors.fill : parent
		visible: true

		itemDelegate : Row {
			width : 300
			height : 20
			spacing : 4
			Image {
				width : 16
				height : 16
				source : "../images/"+ folders.icon(styleData.index)
				fillMode : Image.PreserveAspectFit
			}
			Text {
				width : 200
				height : 18
				text : styleData.value
			}
		}
		TableViewColumn {
			title: "Name"
			role: "fileName"
			width: 300
		}
		model: folders
		rootIndex: folders.rootPathIndex

		onClicked : {
			folders.currentPathIndex = index
		}
		Component.onCompleted : {
		}
	}
}