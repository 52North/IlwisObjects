import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml/datapanel/visualization" as Vis

SplitView {
    id : scMap
	property var lastZoomEnvelope : "" 
	property var oldZoomEnvelope : ""

    width : parent.width
    height : datapane.height

	onHeightChanged : {
		console.debug("xxxxxxx", "scMap", width, "maps", maps.width, "datapane", datapane.width, "modellersplit", modellersplit.width)
	}
    orientation: Qt.Horizontal

	Item {
		width : 250
		height : parent.height
	}

	Vis.MapPanel {
	    id : maps
		//width : parent.width
		height : datapane.height
		showManager : false
	}


	function setRaster(raster){
	    maps.addDataSource("itemid=" + raster.id, raster.name, 'rastercoverage')
	}

	function activeLayerExtentsToolbar() {
		return maptools
	}
}