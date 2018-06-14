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
    width : 270
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
                labelText : qsTr("Background Raster")
                labelWidth : 120
                width : parent.width - 20
                height : 20
                checkFunction : testDrop
                onDropped : {
                    ilwisobjectid = drag.source.ilwisobjectid
                    var filter = "itemid=" + ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "left")
                    models.lastAddedId
                    if ( "manager" in tab.item){
                        tiepointstable.editor.associatedBackgroundMap(tab.item.manager)
                        tab.item.setActiveEditor(tiepointstable)
                        tab.item.manager.addPostDrawer(tiepointstable.editor)

                    }

                }
            }
            Button {
                width : 20
                height : 20
                iconSource : "../../images/view.png"
            }
        }
        Row {
            width : parent.width
            height : 20
            Controls.TextEditLabelPair {
                labelText : qsTr("Reference Raster")
                labelWidth : 120
                width : parent.width - 20
                height : 20
                checkFunction : testDrop
                onDropped : {
                    ilwisobjectid = drag.source.ilwisobjectid
                    var filter = "itemid=" + ilwisobjectid
                    var tab = bigthing.newCatalog(filter ,"rastercoverage",drag.source.url, "right")
                    tiepointstable.editor.linkModels(tab.item.manager)
                }
            }
            Button {
                width : 20
                height : 20
                iconSource : "../../images/view.png"
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
             height : 180
        }

    }
    Item {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        Button {
            id : applybutton
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            width : 70
            text : qsTr("Apply")
            onClicked: {
                dropItem.state = "invisible"
                var points
                var url = applybutton.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
                var createinfo = { name : url, type : "georef", subtype : "tiepoints", tiepoints : points, csy : csypart.content,
                    subpixel : cbsubpixel, description :objectcommon.description}
                objectcreator.createObject(createinfo)
            }

        }
        Button {
            id : closebutton
            anchors.right: applybutton.left
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            width : 70
            text : qsTr("Close")
               onClicked: {
                dropItem.state = "invisible"
            }
        }
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

}


