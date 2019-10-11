import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls 2.3 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import UIContextModel 1.0

import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base


Controls.DropableItem{
    id : dropItem
    width : 275
    height : 0
    clip:true
    property var list
	property var domainValueType

    function setValue(type, value){
        var resource
        if ( type === "georeference"){
            resource = mastercatalog.id2Resource(value, inner)
            grfvalue.content = resource.url
        }
        if ( type === "domain"){
            resource = mastercatalog.id2Resource(value, inner)
            domvalue.content = resource.url
        }
    }

    Rectangle {
        id : inner
        height: parent.height
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        width : parent.width

        Column {
            id : maincolumn
            width : parent.width - 7
            height : 260
            y : 5
            spacing : 2
            x : 4

            EditorHeader{}

            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

            Controls.FilteredTextEditLabelPair{
                id : grfvalue
                labelWidth: 100
                labelText: qsTr("Georeference")
                filterImage: "../images/georeference20.png"
                filterType: "georeference"
                width : parent.width
            }

            Controls.ComboxLabelPair{
                id : domvalue
                labelWidth: 100
                labelText: qsTr("Domain")
                width : parent.width
                content : "value(float)"
				textEditable : true
				filters : ["domain", "itemdomain", "numericdomain"]
				itemModel : ["value(float)", "image(8 bits)","image16(16 bits)","min1to1", "integer(-big to +big)", "count(0..big)", "positiveInteger(1..big)", "distance(0.. float)", "percentage(0..100.0"]
				checkFunction : testDrop

				onContentChanged : {
					domainValueType = mastercatalog.checkValueType(content, true)	
				}
            }
			Controls.TextEditLabelPair {
				id : resid
  			    labelWidth: 100
                labelText: qsTr("Resolution")
                width :220
				visible : domainValueType == "float" || domainValueType == "integer"
				content : domainValueType == "float" ? 0.01 : 1 
			}
            Controls.ComboxLabelPair{
                id : stackdomvalue
                labelWidth: 100
                width : parent.width
                content : "positiveInteger(1..big)"
				textEditable : true
				itemModel : ["value(float)", "image(8 bits)","image16(16 bits)","min1to1", "integer(-big to +big)", "count(0..big)", "positiveInteger(1..big)", "distance(0.. float)", "percentage(0..100.0"]
				currentIndex : 6
				checkFunction : testDrop
            }

            Controls.TextEditLabelPair{
                id :bandsvalue
                labelText: qsTr("Stack Defintion")
                labelWidth: 100
                width : parent.width
                content : "1.."
				callbackFunction : addRasterListAsDomain
            }
            Text{
                height: Global.rowHeight - 4
                text : qsTr("Bands")
                font.bold: true
            }
            Row {
                width : parent.width
                height : 22
                spacing : 6
                ExclusiveGroup {
                    id: group
                }
                RadioButton {
                    id : bywildcard
                    text: qsTr("Per Wildcard")
                    exclusiveGroup: group
                }
                RadioButton {
                    id : bydraganddrop
                    text: qsTr("Drag & Drop")
                    exclusiveGroup: group
                    checked: true
                }

            }
            Item {
                width : parent.width
                height : 110
                Controls.TextEditLabelPair{
                    id : bywildcardtext
                    width : parent.width
                    height : Global.rowHeight
                    labelText: qsTr("Wildcards")
                    labelWidth: 120
                    visible: bywildcard.checked
                    enabled: visible
                }

                Rectangle {
                    id : bands
                    width : parent.width
                    height : 110
                    visible : bydraganddrop.checked
                    enabled : visible
                    DropArea {
					id : dropid
						width : parent.width
						height : parent.height
                        ListModel {
                            id : rasters
                        }

						 ScrollView {
						 	anchors.fill : parent
							anchors.margins: 2
							ListView {
								clip : true
								function bands() {
									var result = ""
									for(var i=0; i < rasters.count; ++i){
										if ( result != "")
											result += ","
										var r = rasters.get(i)
										result += r.path
									}
									return result
								}

								id : rasterlist
								anchors.fill : parent
								currentIndex : -1
								model : rasters
								delegate :  Text { 
									height : 16; text : path
									MouseArea {
										anchors.fill : parent
										onClicked : {
											rasterlist.currentIndex = index
										}
									}
								}
								highlight: Rectangle { color: Global.selectedColor}
							}
						}
                        onDropped : {
                            var idstring = drag.source.ids
                            if ( (idstring && idstring === "") || idstring.indexOf("|") === -1)
                                idstring = drag.source.ilwisobjectid
                            var ids = idstring.split("|")

                            for(var i=0; i < ids.length; ++i ){
                                var id = ids[i]
                                var obj = mastercatalog.id2object(id, rasterlist)
                                var grfid = obj.getProperty("georefid");
                                if ( obj && obj.typeName === "rastercoverage"){
                                    if ( grfvalue.content === ""){
                                        grfvalue.content = obj.getProperty("georeferenceurl");
                                    }
									var ok = mastercatalog.isCompatible(grfvalue.content, obj.id, "georeference");
                                    if (resampleCB.checked || ok)
                                        rasters.append({path : obj.url})
									else
										uicontext.addMessage(qsTr("Georefs are not compatible"),"warning")

									var vt = obj.internalValuetype
									if ( vt === "byte")
										domvalue.currentIndex = 1
									if ( vt === "uint16")
										domvalue.currentIndex = 2
									if ( vt === "uint32" || vt === "uint64")
										domvalue.currentIndex = 5
									if ( vt === "int32" || vt === "int64")
										domvalue.currentIndex = 4
									if ( domvalue.content == "" )
										domvalue.currentIndex = 0
                                }

                            }
                        }

                    }
                    border.width: 1
                    border.color: Global.edgecolor

                }
            }
			Controls.PushButton {
				width : 80
				height: 20
				text : qsTr("delete")
				onClicked : {
					if ( rasterlist.currentIndex >= 0)
						rasters.remove(rasterlist.currentIndex)
						rasterlist.currentIndex = -1
				}
			}
            Item {
                width : 100
                height : Global.rowHeight
                CheckBox{
                    id : resampleCB
                    anchors.fill: parent
                    text : qsTr("Auto resample")
                    checked : false
                }
            }

        }

        function apply(overwrite) {
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ namevalue.content, "rastercoverage")){
                    return false;
                }
            }
             var url = applyButton.currentCatalogCorrectUrl() + "/"+ namevalue.content
            var createInfo = {georeference : grfvalue.content,
                domain : domvalue.content,
				resolution : resid.visible ? resid.content : "",
                stackdefinition : bandsvalue.content,
                type : "rastercoverage",
                name :  url,
                stackdomain : stackdomvalue.content,
                bands : bywildcard.checked ? bywildcardtext.content : rasterlist.bands(),
                autoresample : resampleCB.checked}

            objectcreator.createObject(createInfo)


            return true;
        }


        ApplyCreateButtons {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 25
            anchors.rightMargin: 3
            id : applyButton
            createObject: inner.apply
        }
    }
	function addRasterListAsDomain(url){
		if ( url && url != ""){
			if ( url.indexOf("file://") == 0){
				var parts = url.split("/")
				var name = parts[parts.length - 1]
				var index = name.indexOf(".");
				if ( index != -1){
					name = name.substr(0,index);
				}
				stackdomvalue.content = name + "_stackdomain"
			}
		}
	}

	function testDrop(id, filterTypes){
        if (!id)
            return false
        var obj = mastercatalog.id2object(id, null)
		for ( var i=0; i < filterTypes.length ; ++i) {
			if ( obj.typeName === filterTypes[i]){ 
				obj.suicide()
				return true
			}
		}
		if ( obj)
			obj.suicide()
        return false
    }
}

