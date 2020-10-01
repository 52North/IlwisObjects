import QtQuick 2.7
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import QtQuick.Controls          2.1
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "../../../matrix.js" as Matrix
import "../../../Global.js" as Global
import "../../../controls" as Controls

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan
import Ilwis.Workflow 1.0
import QtQuick.Controls.Material 2.1

Modeller.ModellerWorkArea {
    id : workarea
    property WorkflowModel workflow;
    property var workflowManager
    //property OperationCatalogModel operationCatalog;
    property var operationsList: []
    property var conditionsList: []
    property var rangesList: []

    property var currentItem
	property var currentEdge
    property bool selectState : false
    property bool canvasActive : true;
    property bool dropCondition : false
    property bool dropLoop : false

    onDropConditionChanged: {
        tools.dropLoop(false)
        tools.dropCondition(dropCondition)
    }

    onDropLoopChanged: {
        tools.dropCondition(false)
        tools.dropLoop(dropLoop)
    }
    state : "invisible"

    onWorkflowChanged : {
        state = "visible"
        workarea.visible = true
		graphView.graph().setWorkflow(workflow)
    }

	ToolTip { id: toolTip; timeout: 2500 }

    WorkflowTools{
        id : tools
        z : 10

    }
    SaveAsBox {
        id : savestuff
        anchors.top : tools.bottom
        anchors.topMargin: 2
        state : "invisible"
        clip:true
        z : 1000
    }

	function insertFlow(src, dst, fromParmindex,  toParmIndex) {
		if ( src.type ==  WorkflowGraphNode.Type.Range){
			src = src.rangeValue
		}
		var edge = graphView.graph().insertDataFlowEdge(src,dst, fromParmindex, toParmIndex)
		//edge.item.style.lineType = Qan.EdgeStyle.Curved
		//edge.item.style.lineColor = uicontext.darkColor
	}
	ConnectDialog {
		id: bindingDialog
	}

    DropArea {
        id: canvasDropArea
        anchors.top: tools.bottom
        width : parent.width
        height : parent.height - tools.height
        enabled: true

        function addOperation(objectid,dx,dy,ownerid, parentItem){
            var operation = operations.operation(objectid)
            if ( operation){
                var nodeId
                if ( ownerid === -1)
                    nodeId = workflow.addNode(objectid,{x : dx, y:dy, w:240, h:130,type:'operationnode'})
                else
                    nodeId = workflow.addNode(objectid,{x : dx, y:dy, w:240, h:130,type:'operationnode',owner:ownerid,reason : "operations"})

				var n = graphView.graph().insertOperationNode(nodeId,objectid)
				if ( n){
					n.item.x = dx; n.item.y = dy
					n.item.operation = operation
					n.item.nodeid = nodeId
				}
              
            }
            if ( workflowManager){
                workflowManager.updateRunForm()
            }
            return operation
        }

        function addConnect(objectid,dx,dy,ownerid, parentItem){
            var connectid = operations.operationId("ilwis://operations/connect")
            var operation = addOperation(connectid,dx,dy,ownerid, parentItem)
            var object = mastercatalog.id2Resource(objectid, 0)
            if ( object && operation){
                workflow.setFixedValues(operation.itemid, object.url)
                workflow.createMetadata()
                updateInputNamesList()
                object.suicide()
            }
        }

        onDropped: {
			if (drag.source.type === "singleoperation" || drag.source.type === "workflow"){
				addOperation(drag.source.ilwisobjectid,drag.x,drag.y,-1)
		
			}

	

           /* if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                if (drag.source.type === "workflow") {
                    operations.refresh()
                }
                 var onItem = itemAt(drag.x, drag.y)
                if (!onItem ){
                    addOperation(drag.source.ilwisobjectid,drag.x,drag.y,-1, wfCanvas)

                }else {
                    if ( onItem.type === "conditionitem" || onItem.type === "rangeitem"){
                        if (onItem.inOperationList(drag.y)){
                            var p = wfCanvas.mapToItem(onItem, drag.x, drag.y)
                            var operation = addOperation(drag.source.ilwisobjectid,p.x,p.y,onItem.itemid, onItem)
                            if ( operation){
                                onItem.addToOperationList(currentItem)
                            }
                        }else {
                            onItem.addTestOperation(drag.source.ilwisobjectid)
                        }
                    }
                }
            }else {
                onItem = itemAt(drag.x, drag.y)
                if ( onItem && onItem.type === "rangeitem"){
                    if ( onItem.droppedOnTestPart( drag.x, drag.y)){
                        onItem.addTestDataObject(drag.source.ilwisobjectid, drag.source.message, drag.source.type)
                        return
                    }
                }

               addConnect(drag.source.ilwisobjectid,drag.x,drag.y,-1, wfCanvas)
            }*/
        }



		Qan.GraphView {
			id: graphView
			width : parent.width
			height : parent.height
			navigable   : true
			z : 1

			function graph(){
				return topology
			}

			Component.onCompleted : {
				defaultEdgeStyle = Qan.EdgeStyle.Curved
			}

			graph: WorkflowGraph {
				id: topology
				selectionColor: Material.accent
				connectorEnabled: true
				connectorCreateDefaultEdge: false
				connectorColor: Material.accent
				connectorEdgeColor: Material.accent
				//onConnectorRequestEdgeCreation: { notifyUser("Requesting Edge creation from " + src.label + " to " + ( dst ? dst.label : "UNDEFINED" ) ); bindingDialog.open() }
				onConnectorRequestEdgeCreation: { 
					console.debug("xxxxxxxxx", src, dst)
					bindingDialog.workflowGraph = topology
					bindingDialog.nodeFrom = src
					if ( src.type == 0 && dst.type ==5) {
						bindingDialog.typeFilter = "bool"
					} 
					bindingDialog.nodeTo = dst
				}

				objectName: "graph"
				anchors.fill: parent
				onNodeClicked: {
					currentItem =  node.item
					workflowManager.updateSelectedForm()
					if ( currentEdge)
						currentEdge.item.lineColor = uicontext.darkColor
					currentEdge = null
				}

				onEdgeClicked : {
					if ( currentEdge) {
						currentEdge.item.lineColor = uicontext.darkColor
						currentEdge = null
					} else {
						currentEdge = edge
						currentEdge.item.lineColor = "red"
					}
					if ( currentItem){
						currentItem.selected = false
						currentItem = null
					}
				}

			}

		 Menu {      // Context menu adding items
			id: contextMenu
			property var node: undefined
			MenuItem {
				text: "Insert Condition Group"
				onClicked: {
					var conditionId = workflow.addNode(0,{x : contextMenu.x, y:contextMenu.y, w:400, h:200,type:'conditionnode'})
					let n = topology.insertConditionGroup(conditionId)
					n.label = 'Condition Group'
					n.item.x = contextMenu.x
					n.item.y = contextMenu.y

				}
			}
			MenuItem {
				text: "Insert Range Group"
				onClicked: {
					var conditionId = workflow.addNode(0,{x : contextMenu.x, y:contextMenu.y, w:400, h:200,type:'rangenode'})
					let n = topology.insertRangeGroup(conditionId)
					n.label = 'Range Group'
					n.item.x = contextMenu.x
					n.item.y = contextMenu.y

				}
			}

			onClosed: { // Clean internal state when context menu us closed
				contextMenu.node = undefined
			}
		} // Menu

    onRightClicked: {
        contextMenu.x = pos.x
        contextMenu.y = pos.y
        contextMenu.open()
    }
		}
    }

    WFCanvas{
        id : wfCanvas
        //anchors.top: tools.bottom
        width : 0 //parent.width
        height : 0 // parent.height - tools.height
    }

    Timer {
        interval: 50;
        running: true;
        repeat: true
        onTriggered: {
            if ( !wfCanvas.canvasValid)
                wfCanvas.draw(true)
        }
    }

    function getItem(nodeId){
 
       return null
    }

    function createOperationItem(node){
        var operation = operations.operation(node.operationid)
    	var n = graphView.graph().insertOperationNode( node)
		n.item.nodeid = node.nodeid

		return n;
    }

    function createRangeJunctionItem(rangeNode, nodeData){
        var jnode = rangeNode.addJunction()
		jnode.item.x = nodeData.x
		jnode.item.y = nodeData.y
		jnode.item.width = nodeData.w 
		jnode.item.height = nodeData.h
		jnode.setWorkflowNode(nodeData.nodeid)
        return jnode
    }

	  function createConditionJunctionItem(conditionNode, nodeData){
        var jnode = conditionNode.addJunction()
		jnode.item.x = nodeData.x
		jnode.item.y = nodeData.y
		jnode.item.width = nodeData.w 
		jnode.item.height = nodeData.h
		jnode.setWorkflowNode(nodeData.nodeid)
        return jnode
    }

	function createRangeNode(nodeData){
        var rnode = graphView.graph().insertRangeGroup(nodeData);
        rangesList.push(rnode)
		return rnode
	}

	function createConditionNode(nodeData){
        var cnode = graphView.graph().insertConditionGroup(nodeData);
        conditionsList.push(cnode)
		return cnode
	}


    function recreateFlow(parm,kvp,targetItem){
			
        if ( parm && targetItem && parm.outputIndex !== -1){
            var sourceItem = kvp[parm.outputNodeId]
            if (sourceItem) {

				insertFlow(sourceItem, targetItem,0, parm.outputIndex)
            } else {
                console.debug("Workflow: error node:" + parm.outputNodeId )
            }
       }
    }

    function recreateFlows(targetItem, kvp) {
		var operationNode = workflow.getNode(targetItem.nodeId)
        var parameters = operationNode.parameters
        if (!parameters)
            return

        for(var j=0; j < parameters.length; ++j){
            recreateFlow(parameters[j], kvp,targetItem)
        }
    }

    function recreateRangeFlows(rangeItem, kvp) {
        var node = workflow.getNode(rangeItem.itemid)
        if(!node)
            return

        var parameters = node.parameters
        if (!parameters)
            return

        for(var j=0; j < parameters.length; ++j){
            recreateFlow(parameters[j], kvp,rangeItem,j)
        }

        for(var k=0; k < rangeItem.operationsList.length; ++k){
            recreateFlows(rangeItem.operationsList[k], kvp)
        }
        for( k=0; k < rangeItem.junctionsList.length; ++k){
            recreateFlows(rangeItem.junctionsList[k], kvp)
        }
    }

    function recreateWorkflow() {
	
        if (!workflow)
            return
        var nodes = workflow.getNodes()
        var kvp = {}
        var unlinkedJunctions = []
		var conditions = {}
		
        for(var i=0; i < nodes.length; ++i){
            var node = nodes[i];
            if ( node.type === "conditionnode"){
                currentItem = createConditionNode(node)
                var ownedoperations = node.ownedoperations;
                for(var j = 0;  j < ownedoperations.length; ++j){
					var oper = ownedoperations[j]
                    var opNode = createOperationItem(oper)
                    kvp[opNode.nodeId] = opNode
                    opNode.condition = currentItem                            
					currentItem.addNode2GroupNode(opNode)
					operationsList.push(opNode)
                }
				conditions[currentItem.nodeId] = currentItem
				operationsList.push(currentItem.conditionTest)
            } else if ( node.type === "rangenode"){
				currentItem = createRangeNode(node)
                var ownedoperations = node.ownedoperations;
                for(j = 0;  j < ownedoperations.length; ++j){
					var oper = ownedoperations[j]
                    var opNode = createOperationItem(oper)
                    kvp[opNode.nodeId] = opNode
                    opNode.range = currentItem                            
					currentItem.addNode2GroupNode(opNode)
					operationsList.push(opNode)

                }
                var ownedjunctions = node.ownedjunctions;
                for(j = 0;  j < ownedjunctions.length; ++j){
                    var junctionData = ownedjunctions[j]
                    var junctionNode = createRangeJunctionItem(currentItem, junctionData)
                    kvp[junctionNode.nodeId] = junctionNode
					operationsList.push(junctionNode)

                }

				kvp[currentItem.nodeId] = currentItem

            }else if ( node.type === "operationnode"){
				 
                currentItem = createOperationItem(node)
                if ( currentItem){
					operationsList.push(currentItem)
                    kvp[currentItem.nodeId] = currentItem
                }
            }else if ( node.type === "junctionnode"){
				var condition = conditions[node.linkedcondition]
                currentItem = createConditionJunctionItem(condition, node)
                 kvp[currentItem.nodeId] = currentItem
                 unlinkedJunctions.push(node)
            }
        }
        for(i=0; i < operationsList.length; ++i){
            var operationItem = operationsList[i]
            recreateFlows(operationItem, kvp)

        }
        /*for(i=0; i < conditionsList.length; ++i){
            var conditionItem = conditionsList[i]
            recreateConditionFlow(conditionItem, kvp)
        }

        for(i=0; i < rangesList.length; ++i){
            var rangeItem = rangesList[i]
            recreateRangeFlows(rangeItem, kvp)
        }*/

        for(i=0; i < unlinkedJunctions.length;++i){
			var node = unlinkedJunctions[i]
			var trueCase = node.linkedtrueoperation
			if ( trueCase.outputIndex != -1) {
				var linkid = trueCase.outputNodeId
				if ( linkid != -1){
					var linkedOper = kvp[linkid]
					var junctionNode = kvp[node.nodeid]
					insertFlow(linkedOper, junctionNode, 0,  1)
				}
			}
			var falseCase = node.linkedfalseoperation
			if ( falseCase.outputIndex != -1) {
				var linkid = falseCase.outputNodeId
				if ( linkid != -1){
					var junctionNode = kvp[node.nodeid]
					var linkedOper = kvp[linkid]
					insertFlow(linkedOper, junctionNode, 0,  2)
				}
			}
        }
        if ( workflowManager){
            workflowManager.updateRunForm()
        }
        currentItem = null // nothing is current now
    }

    function showSelectedOperation(item){
        workflowManager.showSelectedOperation(item)
    }

    function updateMetadata() {
        workflowManager.updateMetadata()
    }

    function updateInputNamesList() {
        var oper = currentOperation()
        if ( oper){
            oper.resetInputNames();
        }
    }

    function currentOperation() {
        if ( currentItem && currentItem.type === "operationitem"){
            return currentItem
        }
        return null
    }

    function zoom(amount, absolute, cx, cy){
		graphView.zoom = amount / 100
    }

    function zoomItem(item, dzoom, cx,cy){
    }

    function pan2(amount, direction){
    }

    function pan(px, py){
    }

    function checkContainer(x,y,nodeList, container){
        return null
    }

    function itemAt(x,y){
        return null
    }

    function removeItemFromList(list, nodeid){
        var item
        for(var i=0; i < list.length; ++i)    {
            if ( list[i].itemid == nodeid){
                item = list[i]
                list.splice(i,1)
                break;
            }
        }
        return item
    }

    function removeLinkTo(nodeid){
        for(var j=0; j < operationsList.length; ++j){
            operationsList[j].removeLinkTo(nodeid)
        }
        for(j=0; j < conditionsList.length; ++j){
            conditionsList[j].removeLinkTo(nodeid)
        }
        for(j=0; j < rangesList.length; ++j){
            rangesList[j].removeLinkTo(nodeid)
        }
    }

    function deleteSelectedItem(){
        if ( currentItem){
            workflow.removeNode(currentItem.nodeid)
			topology.removeNode(currentItem.node)
			currentItem = null
				
        }else if ( currentEdge){
			var to = currentEdge.getDestination();
			var from = currentEdge.getSource();
            workflow.deleteFlow(to.nodeId, currentEdge.toParmIndex)
			topology.removeEdge(currentEdge)
			currentEdge = null
            //currentItem.source.removeLinkTo(currentItem.target.itemid)
        }

    }
    function dropSaveBox(xpos) {
        savestuff.x = xpos
        savestuff.toggle()


    }
    function refreshCanvas() {
        wfCanvas.canvasValid = false
    }

    function addFlowConnection(connections, targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        for(var i=0; i < connections.length; ++i){
            var flow = connections[i]
            if ( flow.target.itemid === targetItem.itemid && flow.source.itemid === sourceItem.itemid){
                if ( flow.flowPoints.fromParameterIndex === flowPoints.fromParameterIndex &&
                     flow.flowPoints.toParameterIndex === flowPoints.toParameterIndex   ){
                    return
                }
            }
        }
        connections.push({
                                 "target" : targetItem,
                                 "source" :sourceItem,
                                 "attachtarget" : attachRectIndex,
                                 "attachsource" : attachSource,
                                 "flowPoints" : flowPoints,
                                 "isSelected" : false,
                                 "testindex" : testIndex,
                                 "testparameter" : testParameter,
                                 "type" : "flowconnection"
                             })
    }
    function nextStepWithList(list) {
        for(var i=0; i < list.length; ++i)    {
            if ( list[i].itemid == workflow.lastOperationNode){
                list[i].background = list[i].getBackground(workflow.lastOperationNode)
            }else{
                var item = list[i]
                if ( item.type === "conditionitem"){
                    nextStepWithList(item.operationsList)
                }
                list[i].background = list[i].getBackground(-1)
            }
        }
    }

    function nextStep(){
        nextStepWithList(operationsList)
    }

    function collapseWithList(yesno, list){
        for(var i=0; i < list.length; ++i)    {
            list[i].state = yesno ? "minimized" : "maximized"
            workflow.collapsed(list[i].itemid, list[i].state === "minimized")
            if ( item.type === "conditionitem"){
                nextStepWithList(yesno, item.operationsList)
            }
            if ( item.type === "rangeitem"){
                nextStepWithList(yesno, item.operationsList)
            }
        }
    }

    function collapse(yesno){
        collapseWithList(yesno, operationsList)
    }

    function clearCurrent() {
        if ( !workflow)
            return

        currentItem = null
        dropCondition = false
        dropLoop = false

        for(var i=0; i < operationsList.length; ++i){
            operationsList[i].destroy()
        }
        operationsList = []
        for(i=0; i < conditionsList.length; ++i){
            var item = conditionsList[i]
            item.clear()
        }
        conditionsList = []
        for(i=0; i < rangesList.length; ++i){
            item = rangesList[i]
            item.clear()
        }
        rangesList = []
         workflow = null
        wfCanvas.canvasValid = false
    }

   /* function storeRangeDefinitions() {
        for(var i=0; i < rangesList.length; ++i)    {
            rangesList[i].storeRangeDefinition();
        }
    }*/

	function updateBoundingBoxes() {
		topology.updateBoundingBoxes()
    }
	Component.onCompleted : {
		if ( state == "visible") {
			workarea.visible = true
		} else if ( state == "invisible") {
			workarea.visible = false
        }
	}

	function notifyUser(message) { toolTip.text=message; toolTip.open() }
}
