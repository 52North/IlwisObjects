import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

DropArea {
    id : dropArea
    property alias mouseActions : mouseActions

    onDropped: {
        var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
        addDataSource(resource.url, resource.name, resource.typeName)       
    }


    Rectangle {
        id : mapBorder
        anchors.fill: parent
        anchors.margins: 20
        Controls.DummySpatial{
            id: renderer
            anchors.top: parent.top
            height : parent.height
            width : parent.width
            anchors.margins: 1
            objectName : "layers_" + uicontext.uniqueName()

            Component.onCompleted: {
                renderer.layermanager = layerview.manager
            }

        
            Connections {
                target: mouseActions
                onSetZoomPanButton :{
                    layerview.maptools.panButton.enabled = enablePanAndZoomOut
                    layerview.maptools.zoomoutButton.enabled = enablePanAndZoomOut
                }
            }
            Connections {
                target: mouseActions
                onCheckZoomNormalButton :{
                    layerview.maptools.zoomoutButton.checked = enablePanAndZoomOut
                    layerview.maptools.normalButton.checked = !enablePanAndZoomOut
                }
            }

            Connections {
                target: mouseActions
                onSelectTab :{
                    if ( layerview.tabmodel){
                        if (!layerview.tabmodel.selected)
                               layerview.tabmodel.selectTab()
                    }
                }
            }
            Connections {
                target: mouseActions
                onZoomEnded :{
                    manager.addCommand("setviewextent("+ manager.viewid + "," + envelope + ")");
                    if ( viewmanager){
                        viewmanager.newZoomExtent(envelope)
                    }
                }
            }
		    Controls.LayerExtentMouseActions{
			    id : mouseActions
			    layerManager: manager
			    zoomToExtents: true
			    hasPermanence: false
			    showInfo: true
			    selectiondrawerColor: "basic"
		    } 
        }
    }

    Row{
        anchors.top: parent.top
        anchors.topMargin : 5
        id : toprow
        x : 20
        height : 20
        width : parent.width - 40
        Repeater {
            id : toprowRep
            model : layerview.manager.xGridAxisTop
            width : parent.width
            height : parent.height
            Rectangle {
                height : 15
                width : modelData.size
                Text {
                    height : 15
                    width : modelData.size
                    text : modelData.value
                    x : -contentWidth / 2
                }
            }
        }
    }
 

    Row{
        anchors.bottom: parent.bottom 
        id : bottomrow
        x : 20
        height : 20
        width : parent.width - 40
        Repeater {
            id : bottomrowRep
            model : layerview.manager.xGridAxisBottom
            width : parent.width
            height : parent.height
            Rectangle {
                y : 3
                height : 15
                width : modelData.size
               Text {
                    height : 15
                    width : modelData.size
                    text : modelData.value
                    x : -contentWidth / 2
                }
            }

        }
    }
    Column {
        anchors.left : parent.left 
        id : leftcolumn
        y : 20
        height : parent.height - 40
        width :20
        Repeater {
            id : leftcolumnRep
            model : layerview.manager.yGridAxisLeft
            width : parent.width
            height : parent.height
            Rectangle {
                height : modelData.size
                width : 15
                x : 3
                Text {
                    anchors.left : parent.left
                    text : modelData.value
                    width : 15
                    height : modelData.size
                    transformOrigin: Item.TopLeft
                    rotation : -90
                    y : contentWidth / 2
                }
            }
        }
    }

    Column {
        anchors.right : parent.right 
        id : rightcolumn
        y : 20
        height : parent.height - 40
        width :20
        Repeater {
            id : rightcolumnRep
            model : layerview.manager.yGridAxisRight
            width : parent.width
            height : parent.height
            Rectangle {
                height : modelData.size
                anchors.right : parent.right
                width : 20

                Text {
                    anchors.left : parent.left
                    anchors.leftMargin : 15
                    text : modelData.value
                    width : 15
                    height : modelData.size
                    transformOrigin: Item.TopLeft
                    rotation : 90
                    y : -contentWidth / 2
                }
            }
        }
    }

    function entireMap() {
        layerview.manager.addCommand("setviewextent("+ renderer.viewid + ",entiremap)");
        layerview.manager.refresh()
    }

    function transfer(datapanel){
        var layers = datapanel.manager.layers;
        for(var i = layers.length-1; i > 0; --i){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
            var expr = "adddrawer(" + layerview.manager.viewid + ","+ layers[i].name + ",\"itemid=" + layers[i].id + "\"," + layers[i].typeName + ")"
            layerview.manager.addCommand(expr)
        }
        layerview.manager.refresh()
    }
}

