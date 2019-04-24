import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import LayerManager 1.0
import ModelRegistry 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 310
    height : 0
    x : 4
    clip:true

    Column {
        id : dataarea
        function isCsy(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "coordinatesystem" || tp === "conventionalcoordinatesystem" || tp === "boundsonlycoordinatesystem")
        }

        width : parent.width - 5
        height : 410
        spacing : 4
        EditorHeader{}
        IlwisObjectCommon{
            id : objectcommon
        }
        Row {
            width : parent.width
            height : Global.rowHeight
            Controls.TextEditLabelPair {
                id : csysystem
                labelText : qsTr("Coordinate System")
                labelWidth : 120
                width : parent.width - 20
                height : 20
                checkFunction : testDropCsy
                onDropped : {
                    ilwisobjectid = drag.source.ilwisobjectid
                    tiepointstable.editor.setCoordinateSystem(ilwisobjectid)    

                }
            }
            Button {
                height : 20
                width : 20
                Image {
                    id: name
                    source: "../../images/plus20.png"
                    width : 16
                    height : 16
                    anchors.centerIn: parent
                }
            }
        }
 
        Row {
            width : parent.width
            height : 20
            Controls.TextEditLabelPair {
                id : refraster
                labelText : qsTr("Master Raster")
                labelWidth : 120
                width : parent.width - 20
                height : 20
                checkFunction : testDrop
                onDropped : {
                    ilwisobjectid = drag.source.ilwisobjectid
                    var filter = "itemid=" + ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "right")
                    tiepointstable.editor.linkModels(tab.item.layermanagers[0])
                    tab.item.tabmodel.displayName = "Master Raster"
                }
            }
            Button {
                width : 20
                height : 20
                iconSource : "../../images/view.png"

                onClicked : {
                    var filter = "itemid="+ refraster.ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",refraster.content, "right")
                    tiepointstable.editor.linkModels(tab.item.layermanagers[0])
                    tab.item.tabmodel.displayName = "Master Raster"
                }
            }
        }
       Row {
            width : parent.width
            height : 20
            Controls.TextEditLabelPair {
                id : backraster
                labelText : qsTr("Slave raster")
                labelWidth : 120
                width : parent.width - 20
                height : 20
                checkFunction : testDrop
                onDropped : {
                    ilwisobjectid = drag.source.ilwisobjectid
                    var filter = "itemid=" + ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "left")
                    if ( "layermanagers" in tab.item){
                        tiepointstable.editor.slaveLayerManager(tab.item.layermanagers[0],ilwisobjectid)
                        tab.item.setActiveEditor(tiepointstable)
                        tab.item.activeLayerManager().addPostDrawer(tiepointstable.editor)
                        tab.item.tabmodel.displayName = "Slave Raster"
                    }

                }
            }
            Button {
                width : 20
                height : 20
                iconSource : "../../images/view.png"

                onClicked : {
                    var filter = "itemid="+ backraster.ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",backraster.content, "left")
                    tiepointstable.editor.slaveLayerManager(tab.item.manager,backraster.ilwisobjectid)
                    tab.item.setActiveEditor(tiepointstable)
                    tab.item.activeLayerManager().addPostDrawer(tiepointstable.editor)
                    tab.item.tabmodel.displayName = "Slave Raster"
                }
            }
        }

        Controls.ComboxLabelPair {
            labelText : qsTr("Transformation")
            labelWidth :120
            height : 20
            width : parent.width - 10
            itemModel : ['Affine', 'Conformal', 'Second Order Bilinear', 'Full Second Order', 'Third order', 'Projective']

            onCurrentIndexChanged : {
            if ( currentIndex >= 0 && comboText != ""){
                tiepointstable.editor.transformation = comboText
            }
        }

        }

        TiePointsTable {
            id : tiepointstable
            width : parent.width
            height : 220
        }

    }
    ApplyCreateButtons {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        id : applyBut
        createObject: dropItem.apply
        applyVisible : objectcommon.itemname != "" 

    }
    function apply(overwrite) {
       if (!overwrite){
            if ( mastercatalog.exists("ilwis://internalcatalog/"+ objectcommon.itemname, "georeference")){
                return false;
            }
        }
        var url = applyBut.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
        var createinfo = { name : url, type : "georef", subtype : "tiepoints", georefid : tiepointstable.editor.georefid, description :objectcommon.description}
        objectcreator.createObject(createinfo)
        return true
    }

    function testDrop(id){
        if (!id)
            return false
        var obj = mastercatalog.id2object(id, dropItem)
        if ( obj && obj.typeName === "rastercoverage"){
            return true
        }
        return false
    }

    function testDropCsy(id){
        if (!id)
            return false
        var obj = mastercatalog.id2object(id, dropItem)
        if ( obj && obj.typeName.indexOf("coordinatesystem") != -1){
            return true
        }
        return false
    }

    function allDataSet() {
        return refraster.content != "" && backraster.content != "" && csysystem.content != ""
    }

}


