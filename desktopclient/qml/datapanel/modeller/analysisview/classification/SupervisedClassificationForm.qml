import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls


Column {
    id : colorcomp
    y : 4
	width : 200
	height : 200
	property var bands : []
	property var raster
	spacing : 2



	onRasterChanged : {
	    if ( analisysView && analisysView.view() && raster)
			analisysView.view().setRaster(raster)
	}

	Component {
        id : spectral
        SetMultiSpectralData{}
    }

    Component {
        id : domain
        SetDomain{}
    }

	 Component {
        id : stats
        Statistics{}
    }
	TabView {
	        id : data
            height: 200
            width : parent.width


			Component.onCompleted: {
				addTab(qsTr("Multi Spectral Data"), spectral).active = true
				addTab(qsTr("Domain"), domain).active = true
				addTab(qsTr("Statistics"), stats).active = true
			}
	}

	function selectedBands(red,green,blue){
		console.debug("wwww", red, green, blue)
	}
}

