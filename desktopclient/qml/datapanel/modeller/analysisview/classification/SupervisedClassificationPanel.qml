import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
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
	}

	

	function setRaster(raster){
	    maps.addDataSource("itemid=" + raster.id, raster.name, 'rastercoverage')
		ppp.height = maps.height = datapane.height
    }

	function activeLayerExtentsToolbar() {
		return maptools
	}
}