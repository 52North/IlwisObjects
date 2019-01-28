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
	height : 340
	property var bands : []
	property var ccRaster
	spacing : 2


	onCcRasterChanged : {
	    if ( analisysView && analisysView.view() && ccRaster){
		    var options = 'createtype=colorcomposite,items=' + bands[0].id + '|' +  bands[1].id + '|' +  bands[2].id
			analisysView.view().setRaster(ccRaster, options)
		}
	}

	Component {
        id : spectral
        SetMultiSpectralData{}
    }

    Component {
        id : domain
        ConstructClasses{}
    }

	 Component {
        id : stats
        Statistics{}
    }
	TabView {
	        id : data
            height: parent.height
            width : parent.width


			Component.onCompleted: {
				addTab(qsTr("Multi Spectral Data"), spectral).active = true
				addTab(qsTr("Construct Classes"), domain).active = true
				addTab(qsTr("Statistics"), stats).active = true
			}
	}

	function selectedBands(red,green,blue){
		 var options = red + ',' + green + ',' + blue
		 if ( analisysView && analisysView.view()){
			analisysView.view().setNewBands(options)
		}
	}
}

