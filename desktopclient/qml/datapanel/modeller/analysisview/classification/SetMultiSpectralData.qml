import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

Column {
    height : 200
	width : 300
	spacing : 3
	id : dataSelect
	function selectedBands() {
		return redBand.itemModel[redband.currentIndex] + "|" +
			greenBand.itemModel[redband.currentIndex] + "|" +
			blueBand.itemModel[redband.currentIndex]
	}
	Controls.TextEditLabelPair {
		id :multisr
		labelText : qsTr("Multi Spectral Raster")
		labelWidth : 120
		checkFunction : testDrop

		onContentChanged : {
			if ( content != "" && content.indexOf("://") != -1){
				var query = "resource='" + content +"' and type=8"
				var items = mastercatalog.select(query,"")
				if ( items.length == 1){
					var obj = mastercatalog.id2object(items[0], null)
					if (obj.typeName === "rastercoverage"){
						var isint = Global.isInteger(obj.internalValuetype)
						if ( isint){
							var layerinf = obj.layerInfo
							if ( layerinf.length > 1 ){
								bands = layerinf
								raster = obj
							}
						}
					}
					obj.suicide()
				}
			}
		}
	}

    Controls.ComboxLabelPair {
            id : redband
            labelText : qsTr("Red band")
            labelWidth : 120
            width : parent.width - 5
            role : "name"
            itemModel : bands

			onItemModelChanged : {
			    if ( bands.length > 2){
					dataSelect.select(redband, bands[0].name)
				}
			}
    }

	 Controls.ComboxLabelPair {
            id : greenband
            labelText : qsTr("Green band")
            labelWidth : 120
            width : parent.width - 5
            role : "name"
            itemModel : bands

			onItemModelChanged : {
			    if ( bands.length > 2){
					dataSelect.select(greenband, bands[1].name)
				}
			}

    }
	Controls.ComboxLabelPair {
            id : blueband
            labelText : qsTr("Blue band")
            labelWidth : 120
            width : parent.width - 5
            role : "name"
            itemModel : bands

			onItemModelChanged : {
			    if ( bands.length > 2){
					dataSelect.select(blueband, bands[2].name)
				}
			}
    }

	function select(obj, name){
        var index = -1
        for(var i=0; i < obj.itemModel.length; ++i){
            if ( Global.compareStrings(obj.itemModel[i].name, name, true, true))    {
                index = i
                break
            }
        }
        obj.currentIndex = index
    }

	 function testDrop(id){
        if (!id)
            return false
		var ok = false
        var obj = mastercatalog.id2object(id, null)
        if ( obj) {
			if (obj.typeName === "rastercoverage"){
			    var isint = Global.isInteger(obj.internalValuetype)
				if ( isint){
					var layerinf = obj.layerInfo
					if ( layerinf.length > 1 ){
						ok = true
					}
				}
			}
			obj.suicide()
        }
        return ok
    }
	Component.onCompleted : {
		multisr.content = modelmanager.analisysView.currentAnalysis.multispectralraster
	}
}