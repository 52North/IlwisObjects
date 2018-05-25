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
    property variant subscription
    signal zoomEnded(string envelope) 

    property bool reportExtentsBack : false

    MapArea{
        anchors.fill : parent
        id : maparea

 		Component.onCompleted : {
			layerview.manager.viewArea(maparea)
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
        if ( filter.indexOf("=") !== -1){
            filter = "\"" + filter + "\""
        }
		var cmd = "adddrawer(" + manager.viewid + ",\"\"," + filter + "," + sourceType + ",true)"
        layerview.manager.addCommand(cmd)
        layerview.manager.refresh()
        viewmanager.addDataSource(filter2, sourceName, sourceType)
        return layerview.manager.viewid
    }

    function newExtent(ext){
        layerview.manager.addCommand("setviewextent("+ layerview.manager.viewid + "," + ext + ")");
        layerview.manager.refresh()
    }

}

