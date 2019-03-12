import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

Loader {
    id : chartArea
    width : parent.width
    height : parent.height  - 10
    
	property var tableColumns : []

    onLoaded : {
		chartArea.item.chart = models.createChartModel(chartArea) 
		var scpModel = modellerDataPane.model.analysisModel(0)
		if ( scpModel){
		    chartArea.item.chart.clearChart()
			for(var i=0; i < tableColumns.length; ++i) {
			    console.debug("pppppppppppppppp",tableColumns[i].xbandname)
				var raw = column2raw(tableColumns[i].xbandname)
				var pcolor = scpModel.raw2color(raw)
				var extraParameters = {name : 'fs1', chartType : 'points', color : pcolor}
				chartArea.item.chart.addDataTable(scpModel.featureSpaceTable(),tableColumns[i].xbandname,tableColumns[i].ybandname, extraParameters)
			}
		}
		chartArea.item.showManager = false
    }

	function setBands(bandX, bandY){
		var scpModel = modellerDataPane.model.analysisModel(0)
		if ( scpModel && bandX != bandY){
			console.debug("uuuuuuuuuuuuuu", tableColumns.length)
			scpModel.calcFeatureSpace(bandX, bandY)
			tableColumns = scpModel.tableColumns(bandX, bandY)
			source = models.mainPanelUrl("chart")
		}
	}

	function column2raw(cname){
		var parts = cname.split("_")
		return parts[parts.length - 1]
	}
 }