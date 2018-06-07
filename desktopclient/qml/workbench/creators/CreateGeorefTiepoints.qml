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
            Text{
                id : csyLabel
                width : 120
                height : 20
                text : qsTr("Coordinate System")
                font.bold: true
            }

            Controls.TextFieldDropArea{
                id : csypart
                width : parent.width - csyLabel.width - 23
                height: Global.rowHeight

                canUse: dataarea.isCsy
                readOnly: false
                asName: false
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
   
        Controls.TextEditLabelPair {
            labelText : qsTr("Reference raster")
            labelWidth : 120
            width : parent.width - 10
            height : 20
        }

        Controls.ComboxLabelPair {
            labelText : qsTr("Transformation")
            labelWidth :120
            height : 20
            width : parent.width - 10
            itemModel : ['Affine', 'Conformal', 'Second Order Bilinear', 'Full Second Order', 'Third order', 'Projective']
        }

        TiePointsTable {
             width : parent.width
             height : 120
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
}


