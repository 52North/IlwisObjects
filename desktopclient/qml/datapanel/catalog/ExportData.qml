import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls


Rectangle {
    width: parent.width - 5
    height: parent ? parent.height - 10 : 0
    property var operation
    property string iconName : "../images/copy"
	//color : catalogSplit.backgroundCatalogColor
	property var selectedData : mastercatalog.selectedData

	onSelectedDataChanged : {
		conversionFormats.showRaster = conversionFormats.showFeatures = conversionFormats.showTables = conversionFormats.showCsy = conversionFormats.showDomain = false
		for(var i=0; i < selectedData.length; ++i){
			var obj = selectedData[i]
			var requestedType = obj.typeName
			if(requestedType == "rastercoverage") conversionFormats.showRaster  = true
			if(requestedType === "featurecoverage" || requestedType === "polygoncoverage" || requestedType === "linecoverage" ||requestedType === "pointcoverage") conversionFormats.showFeatures = true
			if(requestedType === "table" || requestedType === "flattable" || requestedType === "attributetable") conversionFormats.showTables = true
			if (requestedType ==="coordinatesystem" || requestedType === "conventionalcoordinatesystem")  conversionFormats.showCsy = true
			if ( requestedType === "domain" || requestedType === "itemdomain" || requestedType === "numericdomain") conversionFormats.showDomain = true;
		}
	}

	SplitView {
	    anchors.fill: parent
	
		function getFormatString(requestedType) {
			return conversionFormats.getFormatString(requestedType)
		}

		Rectangle {
		    clip : true
			id : left
			width: 320
			height: parent.height - 3
			border.width: 1
			border.color: Global.edgecolor
			color :  uicontext.lightestColor
			enabled: currentCatalog ? (currentCatalog.url != "ilwis://internalcatalog" && currentCatalog.url.indexOf("ilwis://system") == -1) : false

			Column {
				anchors.fill: parent
				anchors.leftMargin: 4
				Column {
					width : parent.width
					height : parent.height - 25
					anchors.leftMargin: 4
					spacing : 4

					Text {
						height : 25
						width : 120
						text : qsTr("Conversion formats")
						font.bold: true
						font.pointSize: 10

					}
					DropConversionFormats{
						id : conversionFormats
					}

				}
				Button {
					height : 20
					width : 100
					text : qsTr("Export")

					onClicked : {
						for(var i=0; i < selectedData.length; ++i){
							var obj = selectedData[i]
							var requestedType = obj.typeName
							var formats = conversionFormats.getFormatString(requestedType)
							var inpUrl = obj.rawUrl
							var outputUrl = browser.currentUrl == "" ? mastercatalog.currentUrl : browser.currentUrl
							var objname = obj.name
							objname = objname.split(".")[0]
							if ( basename.content == ""){
								outputUrl += "/" + objname
							}else {
								if ( selectedData.length == 1){
									outputUrl += "/" + basename.content
								}else {
									outputUrl += "/" + objname + "_"+ basename.content
								}
							}
							var ff = formats.split(":")
							var expr = "saveas(\"" + inpUrl + "\",\"" + outputUrl + "\",\"" + ff[1] + "\",\"" + ff[0] + "\")"
							operations.executeoperation(expr,true)

						}

					}
				}
			}
		}
		Column {
			Text {
				height : 18
				font.bold : true
				text : qsTr("Output folder")
				x : 5
			}
		    Layout.minimumWidth: 50
			Layout.fillWidth: true
			height : parent.height - 3
			spacing : 3
			Controls.FolderBrowser {
			    x : 5
				id : browser
				height : parent.height - 50	
			}
			Controls.TextEditLabelPair {
			    id : basename
				x : 5
				width : browser.width 
				height : 20
				labelText : selectedData.length > 1 ? qsTr("Postfix") : qsTr("Output name")
				labelWidth : 90
			}

		}
	}

}