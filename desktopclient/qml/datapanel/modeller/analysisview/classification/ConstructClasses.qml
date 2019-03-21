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
	property var currentAnalysis : 0
	property var domainItems
	property var classification
	property var update : classification ? classification.needUpdate : false
	property var domid : classification ? classification.classficationDomainId : null

	onUpdateChanged : {
		if ( classification){
			selection.model = modellerDataPane.model.analysisModel(currentAnalysis).bandstats(-1)
		}
	}

	onDomidChanged : {
		classification = modellerDataPane.model.analysisModel(currentAnalysis)
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
				//items.model = classification.classificationItems()
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
		    width : 300
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
	Row { 
		width : parent.width
		height : 220
		Column {
			width : parent.width / 2
			height : parent.height
			spacing : 2
			Row {
				width : parent.width
				height : 25

				Connections {
						target: classes
						onIndexChanged :{
							currentClass.model = modellerDataPane.model.analysisModel(currentAnalysis).bandstats(classes.itemModel[classes.currentIndex].raw)	
						}
				}
				Controls.ComboxLabelPair {
						id : classes
						width : 200
						labelWidth : 70
						labelText : qsTr("Class")
						itemModel : modellerDataPane.model.analysisModel(currentAnalysis) ? modellerDataPane.model.analysisModel(currentAnalysis).classificationItems() : null
						role : 'name'
						height : 22

				}
				Button {
					width : 140
					height : 22
					text : qsTr("Clear Current Class")
				}
			}
			SampleStatistics {
				id : currentClass
				width : parent.width
				height : parent.height - 45
			}
		}
		Column {
			width : parent.width / 2
			height : parent.height
			Row {
				width : parent.width
				height : 25

				Text {
					width : 100
					height : 22
					text : qsTr("Selected Pixels")
				}
				Button {
				    y : -3
					width : 180
					height : 22
					text : qsTr("Add Selection to Current Class")

					onClicked : {
					    var raw = classes.itemModel[classes.currentIndex].raw
						modellerDataPane.model.analysisModel(currentAnalysis).calcStats(raw)
						currentClass.model = modellerDataPane.model.analysisModel(currentAnalysis).bandstats(raw)
						analisysView.view().updateClassLayer()

					}
				}
			}
			SampleStatistics {
				id : selection
				width : parent.width
				height : parent.height - 45
			}
		}
	}
}