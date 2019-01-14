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
        console.debug("wwwwwwwww", data.count)
	}

	TabView {
	        id : data
            height: 200
            width : parent.width

			Tab {
				id : spectral
				title : qsTr("Multi Spectral Data")
				active : true

				SetMultiSpectralData {
				    id : ccSelection
					height: 200
					width : 300
				}
			}
			Tab {
				id : domain
				height: parent.height
				width : parent.width
				title : qsTr("Domain")

				SetDomain {
				}
			}

			Tab {
				id : stats
				height: parent.height
				width : parent.width
				title : qsTr("Statistics")

				Statistics {
				}
			}
	}
}

