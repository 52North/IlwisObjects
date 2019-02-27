import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

Column {
    id : domainForm
	anchors.fill : parent
	property var domainItems
	property var classification
	property var update : classification ? classification.needUpdate : false
	property var domid : classification ? classification.classficationDomainId : null

	onUpdateChanged : {
		if ( classification){
			items.model = classification.classificationItems()
		}
	}

	onDomidChanged : {
		classification = modellerDataPane.model.analysisModel(0)
		if ( classification) {
			var obj = mastercatalog.id2object(classification.classficationDomainId, domainForm)
			if ( obj) {
				var rng = analisysView.view().ccRaster.getProperty("numericrange")
				var vmin = rng.split("|")[0]
				var vmax = rng.split("|")[1]
				act.maxv = Math.ceil((vmax - vmin) / 4)
				act.minv = 0
				spDistance.to = act.maxv
				spDistance.from = act.minv
				classification.classficationDomainUrl = obj.url
				items.model = classification.classificationItems()
			}
		}
	}


	spacing : 5
	y : 5
	Row {
	    height :35
		width : 300
		spacing : 4
		x : 4

		Row {
			id : act
		    width : 390
			height : 20
			spacing : 8
			property var minv : 0
			property var maxv : 0
			anchors.verticalCenter: parent.verticalCenter
			Text {
				text :  qsTr("Spectral Distance")
				width : 100
				height : 20
				font.bold : true
			}
			Text {
				width : 35
				height : 20
				text : act.minv
			}
			QC2.Slider {
				id : spDistance
				width : 250
				height : 20
				stepSize : 1

				handle: Rectangle {
					x: spDistance.leftPadding + spDistance.visualPosition * (spDistance.availableWidth - width)
					y: spDistance.topPadding + spDistance.availableHeight / 2 - height / 2
					implicitWidth: 29
					implicitHeight: 24
					radius: 7
					color: spDistance.pressed ? "#f0f0f0" : "#f6f6f6"
					border.color: "#bdbebf"
					Text {
						text : spDistance.value
						anchors.centerIn : parent
						width : parent.width
						height : parent.height
						horizontalAlignment : Text.AlignHCenter
						verticalAlignment : Text.AlignVCenter
						font.pointSize : 8
					}
				}
				onMoved : {
					analisysView.view().setSpectralDistance(value)
				}
			}
			Text {
				width : 45
				height : 20
				text : act.maxv
			}
		}
	}

	TableView {
		width : 570
		height : 180
		id : items
		model : domainitems
		TableViewColumn{
			title : qsTr("Color")
			width : 50
			role : "color"
			delegate: 
				Rectangle {
					width : 50
					height : 20
					border.width : 1
					color : styleData.value
				}
				
		}
		TableViewColumn{
			title : qsTr("Add Selection")
			width : 80
			delegate: 
				Button {
					text: qsTr("Add pixels")
					width : 70
					height : 20
					onClicked : {
						classification.calcStats(items.model[styleData.row].raw)
						items.model = classification.classificationItems()
						analisysView.view().updateClassLayer()
					}
				}
		}
		TableViewColumn{
			title : qsTr("Clear class")
			width : 50
			delegate: 
				Button {
					text: qsTr("clear")
					width : 50
					height : 20
				}
				
		}
		TableViewColumn{
			title : qsTr("Item name")
			role : "name"
			width : 140
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
}