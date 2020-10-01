import QtQuick 2.7
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import QtQuick.Controls          2.1
import QtQuick.Controls 1.4 as QC1
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

Rectangle {
		id: bindingDialog
		width : 350
		height : 140
		property var nodeFrom
		property var nodeTo
		property var workflowGraph
		property string typeFilter : ""
		property int fromParameterIndex : 0
		property int toParameterIndex : 0
		anchors.centerIn: parent
		visible : false
		enabled : visible
	    color : "white"
	    border.width: 2
		border.color: "grey"
		radius : 4
		clip : true
		z : 10

		ListModel {
                id: outModel
        }

		ListModel {
                id: inModel
        }
		onNodeToChanged: {
       
			var inParameterNames = []
			var validTypes = []
			var firstValidIn = -1
			inModel.clear()
			outModel.clear()
			if ( nodeFrom && nodeTo){

				var outParameterNames = [], count=0

				if (nodeFrom.type === WorkflowGraphNode.Type.Operation && nodeTo.type === WorkflowGraphNode.Type.Operation){
					inParameterNames = nodeTo.operation.parameterIndexes(typeFilter,false)
					for (var i in inParameterNames) {
						var type = nodeTo.operation.inputparameterType(i)
						if (validTypes.indexOf(type) === -1) {
							validTypes.push(type)
							outParameterNames = nodeFrom.operation.parameterIndexes(type,true)
							count += outParameterNames.length

							for (var j in outParameterNames) {
								outModel.append({'text': outParameterNames[j]})
								if ( firstValidIn == -1)
									firstValidIn = i
							}
			
							if ( firstValidIn != -1)
								toParameterIndex = firstValidIn;
						}
					}
					if (count === 0) {
						modellerDataPane.addError(1, 'There were no possible matches found between ' + nodeFrom.itemid + ' and ' + nodeTo.itemid)
						//refresh()
					} else {
						
						fillInputModel(nodeFrom.operation.outputparameterType(fromParameterIndex))
					}
				}else if ( nodeFrom.type === WorkflowGraphNode.Type.Operation && nodeTo.type === WorkflowGraphNode.Type.ConditionTestValue){
					outParameterNames = nodeFrom.operation.parameterIndexes("bool",true)
					inModel.append({'text': 'dummy'})
					if ( outParameterNames.length > 0) {

						for (var i in outParameterNames) {
							outModel.append({'text': outParameterNames[i]})
						
						}
					}
				}else if ( nodeFrom.type === WorkflowGraphNode.Type.Operation && nodeTo.type === WorkflowGraphNode.Type.ConditionJunction){
					outParameterNames = nodeFrom.operation.parameterIndexes("",true)
					if ( outParameterNames.length > 0) {
						inModel.append({'text': 'dummy'})
						for (var i in outParameterNames) {
							outModel.append({'text': outParameterNames[i]})
						}
					}
				}else if ( nodeFrom.type === WorkflowGraphNode.Type.Operation && nodeTo.type === WorkflowGraphNode.Type.RangeJunction){
					outParameterNames = nodeFrom.operation.parameterIndexes("?",true)
					if ( outParameterNames.length > 0) {
						inModel.append({'text': 'dummy'})
						for (var i in outParameterNames) {
							outModel.append({'text': outParameterNames[i]})
						}
					}
				}else if ( nodeFrom.type === WorkflowGraphNode.Type.RangeJunction && nodeTo.type === WorkflowGraphNode.Type.Operation){
						var tp = nodeFrom.outputType()
						outModel.append({'text': 'dummy'})
						fillInputModel(tp)

				}else if ( nodeFrom.type === WorkflowGraphNode.Type.ConditionJunction && nodeTo.type === WorkflowGraphNode.Type.Operation){
						outModel.append({'text': 'dummy'})
						fillInputModel('?')

				} else if ( nodeFrom.type === WorkflowGraphNode.Type.RangeValue && nodeTo.type === WorkflowGraphNode.Type.Operation){
						inParameterNames = nodeTo.operation.parameterIndexes("string|number",false)

						for (var i in inParameterNames) {
							inModel.append({'text': inParameterNames[i]})
						
						}
						outModel.append({'text': 'dummy'})
		
				}
				visible = outModel.count != 0 && inModel.count != 0 
				if ( outModel.count == 1 && inModel.count == 1) {// there is only once choice possible
					visible = false // we dont need the form
					addFlow()
				}
			}
		}

		Column {
			x : 3
			height : parent.height 
			width : parent.width - 5
			Item {
				y : 2
				height : 25
				width : parent.width
				Label {
					text : "  Choose parameter to Connect with"
					font.pointSize : 11
					font.bold : true
					anchors.fill : parent
					background : Rectangle {
						anchors.fill : parent
						color : uicontext.lighterColor
					}
				}
			}
			Item {
				x : 10
				width : parent.width - 10
				height : parent.height - 60
				Column {
					width : parent.width
					height : parent.height - 45
					Controls.ComboxLabelPair {
						id : outComboBox
						width : 300
						height : 25
						labelText : qsTr("Source")
						labelWidth : 100
						itemModel : outModel
						currentIndex : 0
						visible : outModel.count > 1
					}

					Controls.ComboxLabelPair {
						id : inComboBox
						width : 300
						height : 25
						labelText : qsTr("Destination")
						labelWidth : 100
						itemModel : inModel
						currentIndex : 0
						visible : inModel.count > 0
					}
				}
			}
			Item {
				anchors.right : parent.right
				width : 155
				height : 30
				Row {
					width : 155
					height : 22
					QC1.Button {
						width : 75
						height : 22
						implicitHeight: 22
						text : qsTr("Cancel")
						onClicked : {
							bindingDialog.visible = false
						}

					}
					QC1.Button {
						width : 75
						height : 22
						text : qsTr("Apply")

						onClicked : {
							addFlow()
						}
					}
				}
			}
		}

		function getIndex(txt){
				var parts = txt.split(":")
				return Number(parts[0])
		}
		function addFlow() {
			bindingDialog.visible = false

			console.debug("yyyyyyyyyy",  outModel.count,  inModel.count)
			if ( outModel.count >= 1 && inModel.count > 1){
				fromParameterIndex =  outModel.count > 1 ? getIndex(outComboBox.content) : 0
				if ( !nodeTo)
					return
				if ( nodeTo.type === WorkflowGraphNode.Type.Operation)
					toParameterIndex = getIndex(inComboBox.content)
				else if ( nodeTo.type === WorkflowGraphNode.Type.ConditionTestValue){
					toParameterIndex = 0
				}
			}
			if ( nodeTo.type == WorkflowGraphNode.Type.ConditionJunction){
							toParameterIndex = workflowGraph.containedInLinkedCondition(nodeFrom.nodeId, nodeTo.nodeId) ? 1 : 2 // 0=condition, 1=truecase, 2=falsecase
				// nodeTo.containedInLinkedCondition(nodeFrom) 
				//nodeTo.dataType = nodeFrom.parameterType(fromParameterIndex, true)
			}
			console.debug("yyyyyyyyyy",  nodeFrom, toParameterIndex)
			if (!nodeFrom) {
				if ( workarea.currentItem.type === WorkflowGraphNode.Type.Operation)
					nodeFrom = workarea.currentItem
			}
			if ( nodeFrom.type != WorkflowGraphNode.Type.RangeValue){ // the ranges definition (its value) is not a node in the underlying workflow graph though it is node in the visual representation (special case)
				if ( workflowGraph.workflow().usableLink(nodeFrom.nodeId, nodeTo.nodeId, fromParameterIndex, toParameterIndex)){
					workflowGraph.setFlow(nodeFrom.nodeId, nodeTo.nodeId,toParameterIndex, fromParameterIndex)
					if(nodeTo.type === WorkflowGraphNode.Type.Operation){
						nodeTo.item.resetInputNames()
					}
					insertFlow(nodeFrom, nodeTo, fromParameterIndex, toParameterIndex)
				}
			}else if ( nodeFrom.type == WorkflowGraphNode.Type.RangeValue && nodeTo.type == WorkflowGraphNode.Type.Operation){
			    workflowGraph.setFlow(nodeFrom.rangeId, nodeTo.nodeId, toParameterIndex, fromParameterIndex)
				insertFlow(nodeFrom, nodeTo, fromParameterIndex, toParameterIndex)
				nodeTo.item.resetInputNames()
			}
			workflowManager.updateRunForm()
		}

		function fillInputModel(type) {
			if (nodeTo) {
				inModel.clear()
				if (nodeFrom.type === WorkflowGraphNode.Type.Operation && nodeTo.type === WorkflowGraphNode.Type.Operation){
					var parameterName = nodeTo.operation.parameterIndexes(type, false)

					for (var i in parameterName) {
						var name = parameterName[i]
						if (!workarea.workflow.hasValueDefined(nodeTo.nodeId, name.split(':')[0])) {
							inModel.append({'text': name})
						}
					}
				}
				if (nodeFrom.type === WorkflowGraphNode.Type.RangeJunction && nodeTo.type === WorkflowGraphNode.Type.Operation){
					var parameterName = nodeTo.operation.parameterIndexes(type, false)

					for (var i in parameterName) {
						var name = parameterName[i]

						if (!workarea.workflow.hasValueDefined(nodeTo.nodeId, name.split(':')[0])) {
							inModel.append({'text': name})
						}
					}
				}
				if (nodeFrom.type === WorkflowGraphNode.Type.ConditionJunction && nodeTo.type === WorkflowGraphNode.Type.Operation){
					var parameterName = nodeTo.operation.parameterIndexes(type, false)

					for (var i in parameterName) {
						var name = parameterName[i]

						if (!workarea.workflow.hasValueDefined(nodeTo.nodeId, name.split(':')[0])) {
							inModel.append({'text': name})
						}
					}
				}
			}
		}
	}