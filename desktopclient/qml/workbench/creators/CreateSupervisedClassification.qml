import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../../datapanel/visualization" as Vis
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0
    x : 4
    clip:true
	property var raster
	property var classDomain
	Column {
        x : 5
        height : 250
        width : parent.width - 5
        id : fields
        spacing : 5

		IlwisObjectCommon{
            id : objectcommon
        }

		Controls.TextEditLabelPair {
		    id :mspectral
			labelText : qsTr("Multi Spectral Raster")
			labelWidth : 120
			checkFunction : mspectral.testDrop

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
								raster = obj
								ok = true
							}
						}
					}
					if (!ok)
						obj.suicide()

				}
				
				return ok
			}
		}
		Controls.TextEditLabelPair {
		    id :msdomain
			labelText : qsTr("Classification domain")
			labelWidth : 120
			checkFunction : msdomain.testDrop

			function testDrop(id){
				if (!id)
					return false
				var ok = false
				var obj = mastercatalog.id2object(id, null)
				console.debug("kkkk", obj, obj.id, obj.typeName, obj.internalValuetype)
				if ( obj) {
					if (obj.typeName === "itemdomain"){
							classDomain = obj
							ok = true
					}
					if (!ok)
						obj.suicide()

				}
				
				return ok
			}
		}
	}
	ApplyCreateButtons {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.right : parent.right
        anchors.rightMargin : 4
        id : applyBut
        createObject: dropItem.apply

    }

	 function apply(overwrite) {
		if( raster && classDomain){
			var rurl = mspectral.content
			var durl = classDomain.url
			var murl = applyBut.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
			if ( rurl != "" && objectcommon.itemname != ""){
				var createInfo = {type : "supervisedclassification", url : murl, domain : durl, raster : rurl, name : objectcommon.itemname, description : objectcommon.description}
				var modelid = objectcreator.createObject(createInfo)
				var filter = "itemid=" + modelid
				bigthing.newCatalog(filter, "model", "","other")
				return true
			}
        }
        return false
    }
}