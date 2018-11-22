import QtQuick 2.10
import QtQuick.Controls 1.2 as QC10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3
import DataPaneModel 1.0
import TabModel 1.0
import "Global.js" as Global
import "./controls" as Controls


TabViewStyle {

    signal expandToWindow(int panelside, int tabindex);
    signal fullSize()

    property string selectColor : "white"
    property string nonselectColor : uicontext.paleColor
    property int tabHeight : 35
	frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }

    tab:
	BorderImage {
		id: name
		property TabModel tabmodel : datapane.tab(side == 1, styleData.index)
		width: parent.width
		height: tabHeight
		source: tabmodel && tabmodel.selected ? "images/tab_big_active2.png" : "images/tab_big_inactive2.png"

		border.left: 5; border.right: 5
		Row {
			id : thetab
			width: parent.width 
			height: tabHeight

			Item {
				id : label
				width : parent.width - minbut.width - rplusbut.width - refreshbut.width - 4
				anchors.bottom: parent.bottom
				height : 35

				Text {
					x : 8
					width : parent.width - 10
					height : 20
					text: tabmodel ? tabmodel.displayName : "?"
					color: tabmodel && tabmodel.selected ? "black" : "#403F3F"
					elide: Text.ElideMiddle
					font.pointSize: tabmodel && tabmodel.selected ? 9 : 8
					font.weight: tabmodel && tabmodel.selected ? Font.Bold : Font.Normal
					anchors.bottom: parent.bottom
					anchors.bottomMargin: 2
				}
				MouseArea  {
					id : mouseArea2
					property variant image
					drag.target: image
					anchors.fill : parent
					onClicked : {
						currentIndex = styleData.index
						datapane.select(side == 1, currentIndex, true)
						dataPanel.activeItem = control.getTab(currentIndex).item
						datatabview.sideChange()

					}
					onDoubleClicked: {
						fullSize()
					}
					onReleased: {
						if ( image){
							image.Drag.drop()
							image.parent = mouseArea2
							image.anchors.fill = mouseArea2
							image.destroy();
						}else
							showTimer.stop()
					}
					onPressed: {
						showTimer.start()
					}

					function createDragObject() {
						image = Qt.createQmlObject('import QtQuick 2.0; Image{
							id : image
							width : 140; height : 35
							source : \"images/tabborderdrag.png\"
							fillMode: Image.PreserveAspectFit
							property var sourcePanel : control.getTab(currentIndex).item

							Drag.keys: "datatab"
							Drag.active: mouseArea2.drag.active
							Drag.hotSpot.x: 10
							Drag.hotSpot.y: 10
							Drag.source : image
							opacity : 0.5

							states: State {
								when: mouseArea2.drag.active
								ParentChange { target: image; parent: root }
								AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
							}
						}', mouseArea2, "dynamicImage");
					}

				}
				Timer{
					id : showTimer
					interval: 250
					onTriggered: {
						mouseArea2.createDragObject()
					}
				}
			}

			Controls.SmallRoundButton {
				id : refreshbut
				anchors.verticalCenter: thetab.verticalCenter
				height : 22
				iconBase : "../images/refresh"
				visible : panelType() == "catalog"
				width : visible ? height : 0
				onClicked: {
					//datapanesplit.closeTab(side == 1, styleData.index)
				}
			}



			Controls.SmallRoundButton {
				anchors.verticalCenter: thetab.verticalCenter
				id : rplusbut
				height : 25
				width : visible ? height : 0
				iconBase : "../images/add"
				visible : panelType() == "catalog"
				onClicked: menu.open()


				Tab2Menu {
					id : menu
				}
			}

			Controls.SmallRoundButton {
				id : minbut
				anchors.verticalCenter: thetab.verticalCenter
				height : 25
				iconBase : "../images/close"
				visible : datapanesplit.totalTabCount() > 1 ? true : false
				width : visible ? height : 0
				onClicked: {
					datapanesplit.closeTab(side == 1, styleData.index)
				}
			}
		}
	}
    
	function panelType() {
		var sidePanel = datapane.activeSide
		if ( sidePanel){
			var tabview = sidePanel.tabview
			if ( tabview && tabview.getTab(tabview.currentIndex)){
				var panel =  tabview.getTab(tabview.currentIndex).item
				if ( panel)
					return panel.panelType
			}
		}
	}
}
