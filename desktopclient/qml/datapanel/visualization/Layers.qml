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

    function addDataSource(filter, sourceName, sourceType){
        var filter2 = filter
        if ( filter.indexOf("=") !== -1 && filter[0] != '\"'){
            filter = "\"" + filter + "\""
        }
        var cmd = "adddrawer(" + layerManager().viewid + ",\"\"," + filter + "," + sourceType + ",true)"

        layerManager().addCommand(cmd)
        layerManager().refresh()
        viewmanager.addDataSource(filter2, sourceName, sourceType)
        maparea.initGeoDrawer()
        layerview.createParameters[layerview.activeSubPanel] = [filter, sourceName, sourceType]
        return layerManager().viewid
    }

   function activePanel(){
        return layerContainer
    }

    function assignLayerManagers() {
        maparea.assignLayerManager(layerview.layermanagers[panelSubIndex])

    }

     function assignLayerManager(lm) {
        maparea.assignLayerManager(lm)
        if ( layerview.createParameters[panelSubIndex]){
        var sz = maparea.mapAreaSize()
            layerview.layermanagers[panelSubIndex].rootLayer.reset(sz.width, sz.height)
            addDataSource(layerview.createParameters[panelSubIndex][0], layerview.createParameters[panelSubIndex][1], layerview.createParameters[panelSubIndex][2])
        }
    }

    function newExtent(ext){
        layerManager().addCommand("setviewextent("+ layerManager().viewid + "," + ext + ")");
        layerManager().refresh()
    }

}

