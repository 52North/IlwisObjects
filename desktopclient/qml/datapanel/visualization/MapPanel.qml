import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import TabModel 1.0
import ModelRegistry 1.0
import "./propertyeditors" as LayerManagement
import "../../controls" as Controls
import "../../Global.js" as Global
import "../../pubsub.js" as PubSub
//import GeoDrawer 1.0


Item {
    id : layerview
    width: parent.width
    height : parent.height
    property TabModel tabmodel
    property string panelType : "mappanel"
    property var createParameters : []
    property var activeEditor : null
    property alias viewmanager : viewmanager
   // property alias maptools: maptools
    property var layermanagers : []
    property bool canSeparate : true
    property string selectiondrawertopic : "main"
    property string selectiondrawertopicoverview : "overview"
    property int activeSubPanel : 0
    property string panelLayout : "1"

    onPanelLayoutChanged : {
        setLayout()
    }

    onActiveSubPanelChanged : {
        viewmanager.activateViewManager(activeSubPanel)
        activateLayerExtentsToolbar(activeSubPanel)
    }

    objectName: uicontext.uniqueName() + "_mappanel"

    Action {
        id : zoomClicked
        onTriggered : {
            if ( activeLayerManager()){
                activeLayerManager().zoomInMode = !activeLayerManager().zoomInMode
                activeLayerManager().zoomOutMode = false
                activeLayerManager().panningMode = false
            }
        }
    }

    Action {
        id : panningClicked
        onTriggered : {
            if ( activeLayerManager()){
                activeLayerManager().panningMode = !activeLayerManager().panningMode
                activeLayerManager().zoomInMode = false
                activeLayerManager().zoomOutMode = false
            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
            entireMap()
            if ( activeLayerManager()){
                activeLayerManager().zoomOutMode = false
                activeLayerManager().panningMode = false
            }
        }
    }
    Action {
        id : refreshClicked
        onTriggered: {

        }
    }
    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( activeLayerManager()){
                activeLayerManager().zoomOutMode = !activeLayerManager().zoomOutMode
                activeLayerManager().zoomInMode = false
                activeLayerManager().panningMode = false
            }
        }
    }
    Action {
        id : normalClicked
        onTriggered : {
            if ( activeLayerManager()){
                activeLayerManager().zoomInMode = false
                activeLayerManager().zoomOutMode = false
                activeLayerManager().panningMode = false
            }
        }
    }
    ComboBox {
        id : layoutChoices
        y : 8
        x : 150
        width : 100
        height : 20
        z : 10
        model : ["1", "2 vertical","2 horizontal","3 horizontal","3 vertical","3: 2 by 1","4"  ]

        onCurrentIndexChanged:{
            if (currentText != panelLayout) { 
                setLayout(currentText)
                panelLayout = currentText
            }
        }
    }
    MapPanelTopToolBar {
        id : maptools1
        visible : true
        enabled : visible
        bid : "1"
        y : 4
    }

     MapPanelTopToolBar {
        id : maptools2
        visible : false
        enabled : visible
        bid : "2"
        y : 4
    }

     MapPanelTopToolBar{
        id : maptools3
        visible : false
        enabled : visible
        bid : "3"
        y : 4
    }

    MapPanelTopToolBar    {
        id : maptools4
        visible : false
        enabled : visible
        bid : "4"
        y : 4
    }

    SplitView {
        anchors.top : activeLayerExtentsToolbar().bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - activeLayerExtentsToolbar().height

        Connections {
            target: layouts.activePanel()
            onZoomEnded :{
                viewmanager.newZoomExtent(envelope)
            }
        }
        handleDelegate: Rectangle{
            width : parent.width
            height : 6
            color : uicontext.lightColor
        }
        LayerPanelLayout {
            id : layouts
            width : parent.width
            height : parent.height - Global.actionBarMaxHeight


        }
        ViewManager{
            id : viewmanager
            height : Global.actionBarHeight
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
        }
    }

    Component.onCompleted: {
        setLayout(panelLayout)
    }

    function setActiveEditor(editor){
        if (activeEditor){
            activeEditor.editor.postDrawerActive = false
        }
        activeEditor = editor
        if ( activeEditor){
            if ( activeEditor.editor)
                activeEditor.editor.postDrawerActive = true
        }
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }


    function addDataSource(filter, sourceName, sourceType){
        if ( filter !== ""){
            if (filter.indexOf("layouttype=")!= -1){
                var parts = filter.split("and")
                filter = parts[0].trim()
                var layouttype = parts.substring(4)
                panelLayout = layouttype.trim()
            }
            var id = layouts.activePanel().addDataSource(filter, sourceName, sourceType)
           
            return id
        }
        return -1
    }

    function entireMap() {
        activeLayerManager().wholeMap()
        var env = activeLayerManager().rootLayer.viewEnvelope
        viewmanager.newZoomExtent(env)
    }

    function transfer(datapanel){
        layers.transfer(datapanel)
        viewmanager.transfer(datapanel)
    }

    function mapClicked(mx,my){
        var parms = {linktype : 'pixelposition', mouseevent : 'clicked', 'column' : mx, 'row' : my, 'georefid' : activeLayerManager().rootLayer.screenGrf.id}
        activeLayerManager().broadCast(parms)
        if ( activeEditor ){
            if ( typeof activeEditor.handleMouseClick == 'function'){
                activeEditor.handleMouseClick(mx,my)
            }
        
        }
    }

   function mousePressed(mx,my){
     
        if ( activeEditor ){
            if ( typeof activeEditor.handleMousePressed == 'function'){
                activeEditor.handleMousePressed(mx,my)
            }
        }
    }

    function mouseMoved(mx,my){
        if ( activeEditor ){
            if ( typeof activeEditor.handleMouseMoved == 'function'){
                activeEditor.handleMouseMoved(mx,my)
            }
        }
    }

  function mouseReleased(mx,my){
        if ( activeEditor ){
            if ( typeof activeEditor.mouseReleased == 'function'){
                activeEditor.mouseReleased(mx,my)
            }
        
        }
    }

  function setLayout(value){
       if ( value === "1"){
            createLayerManagers(1)
            layouts.source = "Layers.qml"
            maptools1.manager = layermanagers[0] 

       }
       if ( value ==="2 vertical"){
            createLayerManagers(2)
            layouts.source = "Layers2v.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
       }
       if ( value === "2 horizontal"){
            createLayerManagers(2)
            layouts.source = "Layers2h.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
       }
       if ( value === "3 horizontal"){
            createLayerManagers(3)
            layouts.source = "Layers3h.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
            maptools3.manager = layermanagers[2]
       }
       if ( value === "3 vertical"){
            createLayerManagers(3)
            layouts.source = "Layers3v.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
            maptools3.manager = layermanagers[2]
       }
       if ( value === "3: 2 by 1"){
            createLayerManagers(3)
            layouts.source = "Layers32b1.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
            maptools3.manager = layermanagers[2]
       }
       if ( value === "4"){
            createLayerManagers(4)
            layouts.source = "Layers4.qml"
            maptools1.manager = layermanagers[0]
            maptools2.manager = layermanagers[1]
            maptools3.manager = layermanagers[2]
            maptools4.manager = layermanagers[3]
       }    
    }

    function createLayerManagers(number){
        if ( number < layermanagers.length){
            layermanagers = layermanagers.slice(0,number-1)
            createParameters = createParameters.slice(0,number-1)
        }else if ( number > layermanagers.length){
            var start = layermanagers.length
            
            for(var i=start; i < number; ++i){
                layermanagers.push(models.createLayerManager(layouts,layerview))
                createParameters.push(null)
            }

         }
         layerview.activeSubPanel = 0
    }

    function activeLayerExtentsToolbar() {
        if ( layerview.activeSubPanel === 0)
            return maptools1
        if ( layerview.activeSubPanel === 1)
            return maptools2
        if ( layerview.activeSubPanel === 2)
            return maptools3
        if ( layerview.activeSubPanel === 3)
            return maptools4
        return null
    }

  function activateLayerExtentsToolbar(index){
        maptools1.visible = false
        maptools2.visible = false
        maptools3.visible = false
        maptools4.visible = false
        if ( index == 0)
            maptools1.visible = true
        if ( index == 1)
            maptools2.visible = true
        if ( index == 2)
            maptools3.visible = true
        if ( index == 3)
            maptools4.visible = true
    }

    function activeLayerManager(){
        if ( activeSubPanel < layermanagers.length ){
            return layermanagers[activeSubPanel]

        }
        return null
    }

}
