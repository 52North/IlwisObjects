import QtQuick              2.7
import QtQuick.Controls     2.0
import QtQuick.Controls 1.0 as QC1
import QtQuick.Layouts      1.3
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects   1.0
import OperationModel 1.0
import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan
import Ilwis.Workflow 1.0

import "../../../Global.js" as Global
import "../../../controls" as Controls

Qan.GroupItem {
     id: groupItem

    default property alias children : template
    container: template.content   // See qan::GroupItem::container property documentation
    onContainerChanged: {
        if (container) {
            groupItem.width = Qt.binding(function() {
                return Math.max(groupItem.minimumGroupWidth, template.content.width)
            })
            groupItem.height = Qt.binding(function() {
                return Math.max(groupItem.minimumGroupHeight, template.content.height)
            })
        }
    }

    //! Show or hide group top left label editor (default to visible).
    property alias labelEditorVisible : template.labelEditorVisible

    //! Show or hide group top left expand button (default to visible).
    property alias expandButtonVisible : template.expandButtonVisible



		Rectangle {
			id: background
			height : 50
			width : 70
			y: 4
			anchors.right : parent.right
			anchors.rightMargin : 4
			border.width: 1
			border.color : Global.edgecolor
			color : mouseid.pressed ? uicontext.lightColor : uicontext.lightestColor
			z : 4
			MouseArea {
				id : mouseid
				anchors.fill : parent
				Image {
					y : 2
					id : iconid
					width : 20
					height : 20
					anchors.horizontalCenter : parent.horizontalCenter
					source : "../../../images/junction.png" 
				}
				Text {
					anchors.top : iconid.bottom
					anchors.topMargin : 3
					width : Math.min(140, parent.width)
					height : parent.height - 4
					anchors.horizontalCenter : parent.horizontalCenter

					text : " Add Junction"
					elide : Text.ElideMiddle
				}

				onClicked : {
					var node = groupItem.node.addJunction()
					var nid = workflow.addNode(0,{x : node.x, y: node.y,w:50, h:50,type:'junctionnode'})
					workflow.addCondition2Junction(groupItem.node.nodeId, nid)
					node.setWorkflowNode(nid)
				}
			}
		}


    Qan.RectGroupTemplate {
        id: template
        width : parent.width
		height : parent.height
        groupItem: parent
        z: 1

        preferredGroupWidth: parent.preferredGroupWidth
        preferredGroupHeight: parent.preferredGroupHeight

    }


    // Emitted by qan::GroupItem when node dragging start
    onNodeDragEnter: { template.onNodeDragEnter() }
    // Emitted by qan::GroupItem when node dragging ends
    onNodeDragLeave: { template.onNodeDragLeave() }

	onNodeAdded : {
		groupItem.node.addNode(nodeItem)
	}

}