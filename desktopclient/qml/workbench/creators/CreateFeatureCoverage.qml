import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0

import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base


Controls.DropableItem{
    id : dropItem
    width : 275
    height : 0
    clip:true
    property var list

    function setValue(type, value){
        var resource
        if ( type === "coordinatesystem" || type == "conventionalcoordinatesystem"){
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
            height : 240
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

            Connections {
                target : csyvalue
                onCreateClicked : {
                    editorList.push({item:creatorContainer, properties:{creator:objectcreator.creatorInfo("coordinatesystem")}})
                }
            }

            Controls.FilteredTextEditLabelPair{
                id : csyvalue
                labelWidth: 100
                labelText: qsTr("Coordinate System")
                filterImage: "../images/coordinate.png"
                filterType: "coordinatesystem"
                width : parent.width
                useCreateButton: true
            }

            Controls.FilteredTextEditLabelPair{
                id : stackdomvalue
                labelWidth: 100
                labelText: qsTr("Stack Domain")
                filterImage: "../images/domain.png"
                filterType: "domain"
                width : parent.width
                useCreateButton: true
                content : "count"
            }

            Controls.TextEditLabelPair{
                id :stackvalues
                labelText: qsTr("Stack Defintion")
                labelWidth: 100
                width : parent.width
                content : "1.."
				//callbackFunction : addRasterListAsDomain
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
                height : 90
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
                    height : 90
                    visible : bydraganddrop.checked
                    enabled : visible
                    DropArea {
                        ListModel {
                            id : featurecoverages
                        }

                        anchors.fill: parent
						 ScrollView {
						 	anchors.fill : parent
							anchors.margins: 2
							ListView {
								clip : true
								function coverages() {
									var result = ""
									for(var i=0; i < featurecoverages.count; ++i){
										if ( result != "")
											result += ","
										var r = featurecoverages.get(i)
										result += r.path
									}
									return result
								}

								id : featurecoveragesList
								anchors.fill : parent
								model : featurecoverages
								delegate :  Text { height : 16; text : path}
							}
						}
                        onDropped : {
                            var idstring = drag.source.ids
                            if ( (idstring && idstring === "") || idstring.indexOf("|") === -1)
                                idstring = drag.source.ilwisobjectid
                            var ids = idstring.split("|")

                            for(var i=0; i < ids.length; ++i ){
                                var id = ids[i]
                                var obj = mastercatalog.id2object(id, featurecoveragesList)
								featurecoverages.append({path : obj.url})
                            }
                        }
                    }
                    border.width: 1
                    border.color: Global.edgecolor

                }
            }
			Controls.LabeledCheckBox {
				id : merge
				width : 220
				labelText : qsTr("Merge coverages")
			}        
		}

        function apply(overwrite) {
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ namevalue.content, "featurecoverage")){
                    return false;
                }
            }
             var url = applyButton.currentCatalogCorrectUrl() + "/"+ namevalue.content
            var createInfo = {coordinatesystem : csyvalue.content,
                type : "featurecoverage",
                name :  url,
                stackdomain : stackdomvalue.content,
				stackdefinition : stackvalues.content,
                featurecoverages : bywildcard.checked ? bywildcardtext.content : featurecoveragesList.coverages(),
				merge : merge.checked
				}


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
}

