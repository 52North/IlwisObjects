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
	property var bands : []
	property var ccRaster

	onCcRasterChanged : {
	    if ( ccRaster){
		    var options = 'createtype=colorcomposite,items=' + bands[0].id + '|' +  bands[1].id + '|' +  bands[2].id
			setRaster(ccRaster, options)
		}
	}

    width : parent.width
    height :parent.height

	onHeightChanged : {
		ppp.height = datapane.height
	}

    orientation: Qt.Horizontal

	FeatureSpaces {
		id : ppp
		width : 300
	}


	Vis.MapPanel {
			id : maps
			width : parent.width - 300
			height : parent.height
			showManager : false
			activeEditor : scMap
	}

	function setRaster(newraster, options){
		var scpModel = modellerDataPane.model.analysisModel(0)
	    maps.addDataSource("itemid=" + newraster.id, newraster.name, 'rastercoverage', options)
		maps.addDataSource("itemid=" + scpModel.classRaster(), 'classraster', 'rastercoverage')
		maps.addDataSource("itemid=" + scpModel.selectionRaster(), 'selection', 'rastercoverage')

		var layer = maps.activeLayerManager().findLayerByName("PixelSelection")
		if(layer){
			layer.vproperty("opacity", 0.6)
		}
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
		var scpModel = modellerDataPane.model.analysisModel(0)
		var rasterPixel = maps.activeLayerManager().rootLayer.screen2raster(ccRaster.id, mx,my)
		scpModel.setGroupStartPoint(rasterPixel)
		var layer = maps.activeLayerManager().findLayerByName("PixelSelection")

		if(layer){
			layer.vproperty("visualattribute|Pixel value|stretchrange", "numericrange:-1|1|1");
			layer.vproperty("updatetextures","?")
			scpModel.needUpdate = true
		}
	}

	function setSpectralDistance(dist) {
		var scpModel = modellerDataPane.model.analysisModel(0)
		scpModel.setSpectralDistance(dist)
		var layer = maps.activeLayerManager().findLayerByName("PixelSelection")
		if(layer){
		    layer.vproperty("visualattribute|Pixel value|stretchrange", "numericrange:-1|1|1");
			layer.vproperty("updatetextures","?")
		}
	}

	function updateClassLayer() {
	var layer = maps.activeLayerManager().findLayerByName("ClassRaster")
	if(layer){
			layer.vproperty("updatetextures","?")
			layer = maps.activeLayerManager().findLayerByName("PixelSelection") // remove selection
			if(layer){
				layer.vproperty("visualattribute|Pixel value|stretchrange", "numericrange:-1|1|1");
				layer.vproperty("updatetextures","?")
			}
		}
	}
	Component.onCompleted : {
	    if ( currentAnalysis){
			var content = currentAnalysis.multispectralraster
			var query = "resource='" + content +"' and type=8"
			var items = mastercatalog.select(query,"")
			if ( items.length == 1){
				var obj = mastercatalog.id2object(items[0], scMap)
				if (obj.typeName === "rastercoverage"){
					var isint = Global.isInteger(obj.internalValuetype)
					if ( isint){
						var layerinf = obj.layerInfo
						if ( layerinf.length > 1 ){
							bands = layerinf
							ccRaster = obj
						}
					}
				}
			}
		}
	}
}