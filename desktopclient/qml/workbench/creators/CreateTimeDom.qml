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
    width : 375
    height : 0
    clip:true


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

            Item {
                id : parentdom
                width : parent.width
                height : 20
                Text{
                    id : parentLabel
                    width : 100
                    height : 20
                    text : qsTr("Parent domain")
                }
                function isNumericDomain(objid){
                    var tp = mastercatalog.id2type(objid)
                    return tp === "numericdomain";
                }

                Controls.TextFieldDropArea{
                    id : parentdomtxt
                    anchors.left : parentLabel.right
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    height: 20

                    canUse: parentdom.isNumericDomain
                    readOnly: false
                    asName: false
                }
            }

            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

        	Controls.DateTimeDropDown{
			    id : minvalue
				width : 370
				labelText : qsTr("Begin Date/Time")
				labelWidth : 100
				z  : 100
			}
        	Controls.DateTimeDropDown{
			    id : maxvalue
				width : 370
				labelText : qsTr("End Date/Time")
				labelWidth : 100
				z : 99
			}
			Row {
				width : parent.width
				height : 20
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
            Controls.TextAreaLabelPair{
                id : descvalue
                labelText: qsTr("Description")
                width : parent.width
                height : 40
                labelWidth: 100
            }
         }
        ApplyCreateButtons {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 28
            anchors.rightMargin: 3
            id : apply2

            createObject: inner.apply
        }

        function apply(overwrite) {
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ namevalue.content, "timedomain")){
                    return false;
                }
            }
			var res = resvalue.content 
			if ( resunit.comboText === "days"){
				res += 'D'
			}
			if ( resunit.comboText === "seconds"){
				res += 's'
			}
			if ( resunit.comboText === "minutes"){
				res += 'm'
			}
			if ( resunit.comboText === "hours"){
				res += 'h'
			}
			if ( resunit.comboText === "months"){
				res += 'M'
			}
			if ( resunit.comboText === "years"){
				res += 'y'
			}
           var url = apply2.currentCatalogCorrectUrl() + "/"+ namevalue.content
            var createInfo = {parentdomain : parentdomtxt.content, type : "timedomain", name :  url, minvalue : minvalue.content, maxvalue : maxvalue.content, resolutionvalue : res, description : descvalue.content}
            var ilwisid = objectcreator.createObject(createInfo)
            if ( ilwisid !== "?" && editorList.depth > 1)
                editorList.currentItem.setValue("domain", ilwisid)
            return true
        }
    }
}

