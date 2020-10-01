import QtQuick              2.7
import QtQuick.Controls     2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts      1.3
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects   1.0
import OperationModel 1.0
import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan

Qan.NodeItem {
    id: wfNodeItem
    Layout.preferredWidth: 250
    Layout.preferredHeight: 150
    width: Layout.preferredWidth
    height: Layout.preferredHeight
    property color styleBackColor: style.backColor
    property color nodeColor
    property color backColor: Material.background
    property var condition : null
	property string type : "operationitem"
	property OperationModel operation : node.operation
	property var nodeid

	onStyleBackColorChanged: nodeColor = Qt.rgba( style.backColor.r, style.backColor.g, style.backColor.b, 0.2 )

	Item {
		id : innerItem
		anchors.fill: parent
		Rectangle {
			id: background
			z: 1
			anchors.fill: parent
			radius: 6
			border.width: 2
			color : workflow.currentNode == nodeid ? uicontext.lighterColor : uicontext.lightestColor
		}

		Column {
			width : parent.width - 4
			height : parent.height - 6
			anchors.top : parent.top
			z : 2
			x : 2
			Row {
				width: parent.width
				height: 30
				Rectangle {
					width : 40
					height : parent.height
					color : uicontext.paleColor
					Text {
						x : 6
						text : wfNodeItem.node.nodeId
						anchors.verticalCenter : parent.verticalCenter
					}
				}
				Rectangle {
					width : parent.width - 40
					height : parent.height
					color : uicontext.lightColor
					Text {
						text : wfNodeItem.node.label
						x : 4
						font.bold : true
						font.pointSize : 12
						anchors.verticalCenter : parent.verticalCenter
						elide: Text.ElideMiddle
					}
				}
			}

			ScrollView {
				id : operationInParameters
				height : 100
				width : parent.width - 20
				clip : true
				x : 6
				ListView{
					id : operationInParametersList
					anchors.fill: parent
					model : getInputNames()
					interactive: false
					delegate:
						Item {
						width : operationInParameters.width
						height: 14
						Row {
							spacing : 4
							width : operationInParameters.width - 15
							height : 10
							Text{
								text : index
								width : 20
								height : 12
								font.pointSize: 10
							}

							Text {
								text : modelData
								width : parent.width - 30
								height : 12
								font.bold: (workflow && workflow.hasValueDefined(wfNodeItem.node.nodeId, index)) ? false : true
								color : textColor(index)
								elide: Text.ElideMiddle
								font.pointSize: 10
							}
						}
					}
				}
			}
		}
		Glow {
			z: 0
			source: background
			anchors.fill: parent
			color: Material.theme === Material.Light ? Qt.lighter( Material.foreground ) : Qt.darker( Material.foreground )
			radius: 6;     samples: 6
			spread: 0.25;   transparentBorder: true
		}
	}

	 function getInputNames(){
        var lst = []
        if ( workflow){
            var node = workflow.getNode(wfNodeItem.node.nodeId);

            if ( node){
                var parms = node["parameters"]

                if ( parms){
                    for(var i=0; i < parms.length; ++i){
                        lst.push(parms[i].label)
                    }
                }
            }
        }
        return lst
    }

	function textColor(parmIndex){
	if (!workflow)
		return "white"

		var node = workflow.getNode(wfNodeItem.node.nodeId)
		if (node){
			var parms = node.parameters
			if ( parms && parmIndex < parms.length ){
				var parmstate = parms[parmIndex].state
				if ( parmstate === 'fixed')
					return "green"
				if ( parmstate === 'calculated')
					return "darkslategray"
			}
		}
		return condition ? "red": "blue"

	}
	function resetInputNames(){
        operationInParametersList.model = null
        operationInParametersList.model = getInputNames()
    }

	function resetColors(){
        if ( condition){
            var valid = workflow.isValidNode(itemid,"all")
            border.width =  valid ? 0 : 1
            border.color = valid ? "transparent" : "red"
            condition.resetColors()
        }
    }

}
