import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml" as Base
import ChartModel 1.0
import DataseriesModel 1.0

Rectangle {
    width: 400
    height: parent ? parent.height - 10 : 0
    property var operation
	property var lastSelection : "" 
	property var selColor : "#ff0000"
	property var markers : []
	property var items : []
	property var colors : []
	property var selectedMarker : -1
	property var chartCanvas

	function rebuildItems(chartview, my) {
		var sortedMarkers = markers
		sortedMarkers.sort(sortNumber)
		var oldItems = items
		items = []

		var prevx = chart.minX
		var prevMarker = -1000
		for(var i=0; i < sortedMarkers.length; ++i ){
			var pnt = chartview.mapToValue(Qt.point(sortedMarkers[i],my),chartview.series(0))
			var newName = "item_" +  i
			if ( i < oldItems.length && oldItems[i].name.indexOf("item_") == -1)
				newName = oldItems[i].name
			var c = Global.rainbow(sortedMarkers.length, i)
			if ( i <  oldItems.length)
				c = oldItems[i].ecolor
			var item = { "minvalue" : prevx.toFixed(2), "maxvalue" : pnt.x.toFixed(2), 
						"name" : newName, "description" : "","markervalue1" : prevMarker,
						"markervalue2" : sortedMarkers[i], "ecolor" : c}
			items.push(item)
			prevx = pnt.x
			prevMarker = sortedMarkers[i]
		}
	}

	function sortNumber(a, b) {
		return a - b
	}

	function deleteGroup(index) {
		var item = items[index]
		var marker1 = item.markervalue1
		var marker2 = item.markervalue2

		for(var i=0; i < markers.length; ++i) {
			if ( markers[i] == marker2){
				markers.splice(i,1)
				break;
			}
		}
		items.splice(index,1)
		itemtable.model = items
		for(var i=0; i < items.length; ++i) {
				chart.sendOverLink({"type" : "interactiveslicing",  "minvalue" : items[i].minvalue, "maxvalue" : items[i].maxvalue,"color" : items[i].ecolor, "first" : i==0})
		}
		if ( items.length == 0)
			chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
	}

	function handleClick(chartview, mx,my, mode){
		if (!activeSelect.checked)
			return

		var minx = chart.minX
		var maxx = chart.maxX
		var miny = chart.minYLeft
		var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
		var pnt2 = chartview.mapToPosition(Qt.point(maxx,miny),chartview.series(0))
		if ( pnt1.x < mx && pnt2.x > mx) {
			if ( mode == "pressed") {
				selectedMarker = -1
				for(var i=0; i < markers.length; ++i ){
					if ( Math.abs(markers[i] - mx) < 4){
						selectedMarker = i
						break
					}
				}
				if ( selectedMarker == -1){
					markers.push(mx)
					rebuildItems(chartview, my)
					itemtable.model = items
				}
			}else if ( mode == "moved" && selectedMarker != -1){
				markers[selectedMarker]  = mx
				rebuildItems(chartview, my)
				itemtable.model = items
			}
			for(var i=0; i < items.length; ++i) {
				chart.sendOverLink({"type" : "interactiveslicing",  "minvalue" : items[i].minvalue, "maxvalue" : items[i].maxvalue,"color" :  items[i].ecolor, "first" : i==0})
			}
			
		}
	}

	function handleUI(chartview, pcanvas) {
		if (!activeSelect.checked)
			return

	
		chartCanvas = pcanvas
		var ctx = pcanvas.getContext("2d")
		var minx = chart.minX
		var maxx = chart.maxX
		var miny = chart.minYLeft
		var maxy = chart.maxYLeft

		for(var i=0; i < markers.length; ++i) {
			var pnt1 = chartview.mapToPosition(Qt.point(minx,miny),chartview.series(0))
			var pnt2 = chartview.mapToPosition(Qt.point(minx,maxy),chartview.series(0))
			ctx.beginPath()
			ctx.strokeStyle =  i != selectedMarker ? "black" : "blue"
			ctx.moveTo(markers[i], pnt1.y)
			ctx.lineTo(markers[i], pnt2.y)
			ctx.stroke()
		}
	}

	Controls.ColorPicker2 {
		property var basey : 0
		id : picker
		width :240
		height : 20
		visible : false

		Component.onCompleted : {
			basey = picker.y
		}
	}

	Column {
	   height : 160
	   width : 390
	   x : 5
	   spacing : 6
	   Row {
			width : parent.width
			height : 22
			spacing : 6
		    CheckBox {
				id : activeSelect
				width : 120
				height : 20
				text : qsTr("Activate Selections")

				onClicked : {
					if (!checked){
						lastSelection = ""
						if ( chartCanvas) {
							chartCanvas.requestPaint()
							chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						}
					}else{
						markers = []
						chart.sendOverLink({"selectionmode" : "none" , "x" : 0, "y" : 0,"color" : "" })
						lastSelection = "at"
					}
				}
			}
		}
		TableView {
			visible : activeSelect.checked
			id : itemtable
			width : parent.width
			height : parent.height - 45
			property var limitwidth : 35
			TableViewColumn{
				title : qsTr("Action");
				width : 50
				delegate: Controls.PushButton {
						width : 60
						height : 20
						text: qsTr("Delete")

						onClicked : {
							deleteGroup(styleData.row)
							chartCanvas.requestPaint()
						}
				}
			}
			TableViewColumn{
				title : qsTr("Item name");
				role : "name"
				width : 80
				delegate:  Component{
					Controls.TableTextField {
						onAccepted : {
								items[styleData.row].name = text
						}
						onActiveFocusChanged : {
							if ( activeFocus){
								itemtable.currentRow = styleData.row
							}else {
								if ( text != styleData.value)
									items[styleData.row].name = text
							}
						}
					}
                }
			}

			TableViewColumn{
				title : qsTr("Minimum Value");
				role : "minvalue"
				width : itemtable.limitwidth
				delegate: Component{
					Text {
						text: styleData.value
						verticalAlignment:Text.AlignVCenter
						font.pixelSize: 10
						elide: Text.ElideRight

					}
				}
			}
			TableViewColumn{
				title : qsTr("MaximumValue");
				role : "maxvalue"
				width : itemtable.limitwidth
				delegate: Component{
					Text {
						text: styleData.value
						verticalAlignment:Text.AlignVCenter
						font.pixelSize: 10
						elide: Text.ElideRight
					}
				}
			}
			TableViewColumn{
				title : qsTr("Color");
				role : "ecolor"
				width : 190
				delegate: Rectangle {
					id : local
					width :190
					height : 18
					anchors.verticalCenter: parent.verticalCenter
					color : styleData.value

					function selectColor(clr){
						items[styleData.row].ecolor = clr
						local.color = clr;
						picker.visible = false
						var index = styleData.row
						chart.sendOverLink({"type" : "interactiveslicing",  "minvalue" : items[index].minvalue, "maxvalue" : items[index].maxvalue,"color" :  clr, "first" : index==0})
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							var p = picker.mapFromItem(local, local.x, local.y) 
							picker.x = picker.x + p.x
							picker.y = picker.y + p.y + 2
							picker.width = local.width - 5
							picker.initialColor = local.color
							picker.visible = true
							picker.closeCallBack = local.selectColor
						}
					}
				}
			}
			TableViewColumn{
				title : qsTr("Description");
				role : "description"
				width : 200
				delegate: Component{
					Text {
						text: styleData.value
						verticalAlignment:Text.AlignVCenter
						font.pixelSize: 10
						elide: Text.ElideMiddle
					}
				}
			}
			rowDelegate: Rectangle {
				id : rowdelegate
				height : 22
				color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
			}
		}
		/*Controls.PushButton {
			visible : activeSelect.checked
			width : 120
			height : 22
			text : qsTr("Delete boundary")
		}*/
	}
}

