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
    property var localmanager

    function mapAreaSize() {
        return {"width" : renderer.width , "height" : renderer.height}
    }

    onDropped: {
        var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
        layerview.activeSubPanel  = layerContainer.panelSubIndex
        var filter = "itemid=" + resource.id
        addDataSource(filter, resource.name, resource.typeName)       
    }



    Rectangle {
        id : backArea
        anchors.fill: parent
        color : layerContainer.panelSubIndex == layerview.activeSubPanel ? "#f2f2f2" : "white"

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

                Connections {
                    target: mouseActions
                    onSetZoomPanButton :{
                        layerview.activeLayerExtentsToolbar().panButton.enabled = enablePanAndZoomOut
                        layerview.activeLayerExtentsToolbar().zoomoutButton.enabled = enablePanAndZoomOut
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckZoomNormalButton :{
                        layerview.activeLayerExtentsToolbar().zoomoutButton.checked = enablePanAndZoomOut
                        layerview.activeLayerExtentsToolbar().normalButton.checked = !enablePanAndZoomOut
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckNormalButton :{
                        layerview.activeLayerExtentsToolbar().normalButton.checked = checked
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckZoomOutButton :{
                        layerview.activeLayerExtentsToolbar().zoomoutButton.checked = checked
                    }
                }
                Connections {
                    target: mouseActions
                    onCheckPanButton :{
                        layerview.activeLayerExtentsToolbar().panButton.checked = checked
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
                    onClick :{
                        layerview.activeSubPanel  = layerContainer.panelSubIndex
                        layerview.mapClicked(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onMouseMoved :{
                        layerview.mouseMoved(mx,my)   
                    }
                }

                Connections {
                    target: mouseActions
                    onMouseReleased :{
                        layerview.mouseReleased(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onMousePressed :{
                        layerview.mousePressed(mx,my)   
                    }
                }
                Connections {
                    target: mouseActions
                    onZoomEnded :{
                        layerManager().addCommand("setviewextent("+ layerManager().viewid + "," + envelope + ")");
                        broadCastNewExtent(layerManager(), envelope)

                        if ( viewmanager){
                            viewmanager.newZoomExtent(envelope)
                        }
                    }
                }
		        Controls.LayerExtentMouseActions{
			        id : mouseActions
			        layerManager: layerContainer.layerManager()
			        //maptools : layerview.activeLayerExtentsToolbar()
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
                model : localmanager ? localmanager.xGridAxisTop : null
                width : parent.width
                height : parent.height
                Rectangle {
                    height : 15
                    width : modelData.size
                    color : backArea.color
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
                model : localmanager ? localmanager.xGridAxisBottom : null
                width : parent.width
                height : parent.height
                Rectangle {
                    y : 3
                    height : 15
                    width : modelData.size
                    color : backArea.color
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
                model : localmanager ? localmanager.yGridAxisLeft : null
                width : parent.width
                height : parent.height
                Rectangle {
                    height : modelData.size
                    width : 15
                    x : 3
                    color : backArea.color
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
                model : localmanager ? localmanager.yGridAxisRight : null
                width : parent.width
                height : parent.height
                Rectangle {
                    height : modelData.size
                    anchors.right : parent.right
                    width : 20
                   color : backArea.color
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
    }

    function changeSubPanel() {
        if ( layerview.activeSubPanel  != layerContainer.panelSubIndex){
            layerview.activeSubPanel  = layerContainer.panelSubIndex
            return true
        }
        return false
    }

    function assignLayerManager(newmanager){
        localmanager = newmanager
    }

    function initGeoDrawer() {
        if (!renderer.layermanager )
            renderer.layermanager = localmanager
    }

    function entireMap() {
        layerManager().addCommand("setviewextent("+ renderer.viewid + ",entiremap)");
        layerManager().refresh()
    }

    function transfer(datapanel){
        var layers = localmanager.layers;
        for(var i = layers.length-1; i > 0; --i){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
            var expr = "adddrawer(" + localmanager.viewid + ","+ layers[i].name + ",\"itemid=" + layers[i].id + "\"," + layers[i].typeName + ")"
            localmanager.addCommand(expr)
        }
        localmanager.refresh()
    }

}

