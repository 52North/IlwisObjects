import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Item {
    id : layerContainer
    objectName: "layerContainer_ui"  + uicontext.uniqueName()
    property alias layerContainerId : layerContainer
    signal zoomEnded(string envelope) 
    property var panelSubIndex : 0

   function layerManager() {
        if ( layerContainer.panelSubIndex < layerview.layermanagers.length){
            return layerview.layermanagers[layerContainer.panelSubIndex]
        }
        return null
    }

    property bool reportExtentsBack : false

 
    MapArea{
        anchors.fill : parent
        id : maparea

 		Component.onCompleted : {
            if (layerManager())
			    layerManager().viewArea(maparea)
		}
    }

 
    MapScrollers {
        anchors.fill: parent
        id : mapScrollers
    }

    function rootLayer() {
        maparea.rootLayer
    }

    function drawer(){
        return maparea.manager
    }

    function maparea(){
        return maparea
    }

    function transfer(mappane){
        maparea.transfer(mappane)
    }

    function addDataSource(filter, sourceName, sourceType, options){
        var filter2 = filter
        if ( filter.indexOf("=") !== -1 && filter[0] != '\"'){
            filter = "\"" + filter + "\""
        }
        var cmd = "adddrawer(" + layerManager().viewid + ",\"\"," + filter + "," + sourceType + ",true,\"" + (options !== undefined ? options : "") +  "\")"

        layerManager().addCommand(cmd)
        layerManager().refresh()
		if (typeof viewmanager !== 'undefined')
			viewmanager.addDataSource(filter2, sourceName, sourceType, options)
        maparea.initGeoDrawer()
        layerview.createParameters[layerview.activeSubPanel] = [filter, sourceName, sourceType]
		models.lastAddedId = layerManager().viewid
		console.debug("xxxxxxxxx", cmd)
        return layerManager().viewid
    }

   function activePanel(){
        return layerContainer
    }

    function assignLayerManagers() {
        assignLayerManager(layerview.layermanagers[panelSubIndex])

    }

     function assignLayerManager(lm) {
        maparea.assignLayerManager(lm)
        if ( layerview.createParameters[panelSubIndex]){
            var sz = maparea.mapAreaSize()
            layerview.layermanagers[panelSubIndex].rootLayer.reset(sz.width, sz.height)
            maparea.initGeoDrawer()
        }
    }

    function newExtent(ext){
        layerManager().addCommand("setviewextent("+ layerManager().viewid + "," + ext + ")");
        layerManager().refresh()
    }

}

