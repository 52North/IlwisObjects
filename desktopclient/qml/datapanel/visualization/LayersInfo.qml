import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Column {
    id : layersinfo
    width : viewmanager.width
    height: viewmanager.height
    property string iconName : "../images/info"

    function iconSource(name) {
        if ( name === "" || name === undefined){
            return ""
		 }
         var iconP = "../../images/" + name
         return iconP

     }
    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Attribute information on Mouse Position")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    GridView{
        width : parent.width
        height : parent.height - header.height - 2
        model : activeLayerManager().rootLayer.layerInfoItems //manager ? manager.rootLayer.layerInfoItems : null

        cellWidth : 240
        cellHeight : 15
        flow: GridView.FlowTopToBottom
        clip : true
        delegate: Component{
            Item{
                width : 260
                height : 15
                Row {
					width : parent.width
					height : 15
                    spacing: 3
                    Image {
                        source : iconSource(modelData.icon)
                        width : 14
                        height : 14
                    }

                    Text{
                        text : modelData.name
                        width : 100
						height : 15
                        font.bold : true
                        color: modelData.header ? "darkblue" : "black"
                        elide: Text.ElideMiddle
                    }
                    Text {
                        text : modelData.value
                        width : 140
						height : 15
                        clip : true
                        color: modelData.header ? "darkblue" : "black"
                        font.bold : modelData.header ? true : false
                        elide: Text.ElideMiddle

                    }
                }
            }
        }
    }
}

