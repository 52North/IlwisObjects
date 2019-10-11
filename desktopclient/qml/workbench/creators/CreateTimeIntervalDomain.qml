import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls 2.5 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import QtQuick.Window 2.12
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0

    clip:true
	property var editorEnabled : false
	function useWholeDays() {
		var unit = commonpart.additionalFields.item.unit
		return unit == "days" || unit == "months" || unit == "years" || unit == "weeks"
	}

	Component {
	    id : itemresolution
		Row {
			width : parent.width
			height : 20
			property var value : resvalue.content
			property var unit : resunit.content
			Controls.TextEditLabelPair {
				id : resvalue
				width : 200
				labelWidth: 100
				height : 20
				content : "1"
				labelText: qsTr("Resolution")
			}
			Controls.ComboxLabelPair{
				id : resunit
				labelText: ""
				labelWidth: 100
				width : parent.width - 200
				itemModel : ["seconds", "minutes","hours","days", "weeks","months", "weeks"]
				currentIndex : 3

			}
		}
	}

	QC2.ScrollView {
		width : parent.width
		height : Math.min(Screen.height - 290, 600)
		QC2.ScrollBar.horizontal.policy: QC2.ScrollBar.AlwaysOff
		Rectangle {
			id : container
			implicitHeight : 590
			implicitWidth : 390
			border.width : 1
			border.color : Global.edgecolor
			radius: 5
			property var parentDomain
			property var itemArray : []

			ItemDomainCommonPart{
				id : commonpart
				domaintype: "itemdomain"
				valuetype: "Time Interval"
				newItemEditor: "AddNewTimeInterval.qml"
				domitems.source : "IntervalTable.qml"
				additionalFields.sourceComponent : itemresolution
			}

			function apply(overwrite) {
				var itemstring = ""
				if ( commonpart.domitems.item.model){
					if (!overwrite){
						if ( mastercatalog.exists("ilwis://internalcatalog/"+ commonpart.name, "domain")){
							return false;
						}
					}
					for(var i = 0; i < commonpart.domitems.item.model.length; ++i){
						if (itemstring !== "")
							itemstring += "|"
						itemstring += commonpart.domitems.item.model[i].name;
						if (  commonpart.parentdomain == "" ){
							itemstring += "|"+ commonpart.domitems.item.model[i].minvalue;
							itemstring += "|"+ commonpart.domitems.item.model[i].maxvalue;
							itemstring += "|"+ commonpart.domitems.item.model[i].code;
							itemstring += "|"+ commonpart.domitems.item.model[i].description;

						}
					}
					var url = applyButton.currentCatalogCorrectUrl() + "/"+ commonpart.name
					var res = commonpart.additionalFields.item.value
					var unit = commonpart.additionalFields.item.unit
					if (unit === "days"){
						res += 'D'
					}
					if ( unit === "seconds"){
						res += 's'
					}
					if ( unit === "minutes"){
						res += 'm'
					}
					if ( unit === "hours"){
						res += 'h'
					}
					if ( unit === "months"){
						res += 'M'
					}
					if ( unit === "years"){
						res += 'Y'
					}
					var createInfo = {parentdomain : commonpart.parentdomain, type : "itemdomain", valuetype : "timeinterval", name : url, resolution : res,  items : itemstring, description : commonpart.description,strict : commonpart.strict}
					applyButton.createdId = objectcreator.createObject(createInfo)
				}
				return true
			}

			ApplyCreateButtons {
				width : parent.width
				height : 60
				anchors.bottom: parent.bottom
				anchors.bottomMargin: 8
				id : applyButton
				createObject: container.apply
				visible : !editorEnabled

			}

		}

	}
		function addDomainItems(items,clear){
		if ( clear)
			container.itemArray = []
		for(var i = 0; i < items.length; ++i){
			if ( items[i].name === "")
				continue;
            var minv = uicontext.isoString2julianTime(items[i].minvalue)
			var maxv = uicontext.isoString2julianTime(items[i].maxvalue)
			if ( minv > maxv){
				continue;
			}
			var illegal = false
			for ( var j = 0; j < container.itemArray.length; ++j){
				// double names allowed
				if( items[i].name === container.itemArray[j].name){
					uicontext.addMessage(qsTr("Duplicate names are not allowed"),"warning")
					illegal = true
				}
				// no double codes allowed unless its empty
				if( items[i].code !== "" && (items[i].code === container.itemArray[j].code)){
					uicontext.addMessage(qsTr("Duplicate codes are not allowed"),"warning")
					illegal = true
				}
				if ( illegal)
					break
			}
			if ( !illegal){
				var found = false
				for(var n=0; n < container.itemArray.length; ++n){
					var cminv = uicontext.isoString2julianTime(container.itemArray[n].minvalue)
					if ( minv > cminv){
						container.itemArray.splice(n,0, items[i])
						found = true
						break
					}
				}
				if (!found)
					container.itemArray.push(items[i])
			}
		}
		commonpart.domitems.item.model = container.itemArray
		commonpart.domitems.item.limitwidth = 100
	}

}




