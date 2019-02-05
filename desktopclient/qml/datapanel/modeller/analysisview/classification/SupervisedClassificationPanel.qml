import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import AnalysisModel 1.0
import "../../../../Global.js" as Global
import "../../../../controls" as Controls
import "../../../../datapanel/visualization" as Vis

SplitView {
    id : scMap
	property var lastZoomEnvelope : "" 
	property var oldZoomEnvelope : ""

    width : parent.width
    height :parent.height

	onHeightChanged : {
		ppp.height = datapane.height
	}

    orientation: Qt.Horizontal

	Item {
	id : ppp
		width : 250
		height : datapane.height
	}


	Vis.MapPanel {
			id : maps
			width : parent.width - 250
			height : parent.height
			showManager : false
			activeEditor : scMap
	}

	function setRaster(newraster, options){
		var scpModel = modellerDataPane.model.analysisModel(0)
	    maps.addDataSource("itemid=" + newraster.id, newraster.name, 'rastercoverage', options)
		maps.addDataSource("itemid=" + scpModel.selectionRaster(), 'selection', 'rastercoverage')

		ppp.height = maps.height = datapane.height
	}

	function setNewBands(bands){
		var cmd = "setcolorcompositebands(" + maps.activeLayerManager().viewid + ",5," +  bands + ")"
		maps.activeLayerManager().addCommand(cmd)
	}

	function activeLayerExtentsToolbar() {
		return maptools
	}

	function handleMousePressed(mx,my) {
	    console.debug(mx,my)
		var scpModel = modellerDataPane.model.analysisModel(0)
		var rasterPixel = maps.activeLayerManager().rootLayer.screen2raster(analysisManager.form.ccRaster.id, mx,my)
		scpModel.setGroupStartPoint(rasterPixel)
	}

	function setSpectralDistance(dist) {
		var scpModel = modellerDataPane.model.analysisModel(0)
		scpModel.setSpectralDistance(dist)
	}
}