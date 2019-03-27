import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
			store()
        }
    }
    color : uicontext.lightestColor


    Column {
        id : texts
        x: 4
        y : 10
        width : parent.width
        height : Global.rowHeight * 4
        spacing: 3
        Controls.TextEditLabelPair{ labelText:   qsTr("X Axis Domain"); content : getProperty("xaxisdomain"); labelWidth: 120;width : parent.width; readOnly: true}
        Controls.TextEditLabelPair{ labelText : qsTr("Y Axis Domain"); content : getProperty("yaxisdomain"); labelWidth: 120;width : parent.width; readOnly: true}
        Controls.TextEditLabelPair{ labelText : qsTr("Combination domain"); content : getProperty("combodomain"); labelWidth: 120;width : parent.width; readOnly: true}


    }

    GridView {
        anchors.top : texts.bottom
        anchors.topMargin: 4
        id : grid
        width : 120* (parseInt(getProperty("xaxisvalueslength")) + 1)
        height: 20* (parseInt(getProperty("yaxisvalueslength")) + 1)
        cellWidth: 120
        cellHeight: 20
		property var currentPos
        model : getProperty("combinationmatrix").split("|")
        delegate: Item {
			id : delg
			width : 120
			height : 20
			Rectangle {
				id : readOnlyField
				anchors.fill : parent
				border.width: 1
				border.color: Global.edgecolor
				color : index == 0 ? uicontext.lightColor :  "white"
				visible : !writeField.visible
				enabled : visible
				Text{
				    id : readField
					x: 2
					y : 2
					height : 18
					width : parent.width - 3
					text : modelData
					clip : true
					elide : Text.ElideMiddle

					Component.onCompleted : {
						text : modelData
					}
				}
				MouseArea {
				    id : mouseArea
					anchors.fill : parent
					onClicked : {
						if ( propertyForm.editable) {
							writeField.visible = true
							var point = grid.mapFromItem(mouseArea, mouseX, mouseY)
							grid.currentPos = currentPosition( point)
							var items = getProperty("combodomainitems")
							writeField.model = items.split("|")
							var ci = writeField.find(readField.text)
							if ( ci >= 0){
								writeField.currentIndex = ci
							}
						}
					}
				}
			}
			ComboBox {
			    id : writeField
				anchors.fill : parent
				visible : false && propertyForm.editable
				enabled : visible

				onActivated: {
					currentIndex = index
					readField.text = currentText
					if ( grid.currentPos) {
						var values = { "x": grid.currentPos.x, "y" : grid.currentPos.y, "value" : currentText}
						setProperty("combinationmatrixvalues",values)
						grid.currentPos = null
					}
					writeField.visible = false
				}
			}
		}

    }
	function currentPosition(point){
		var xsz = parseInt(getProperty("xaxisvalueslength")) + 1
		var ysz = parseInt(getProperty("yaxisvalueslength")) + 1
		var idx = grid.indexAt(point.x,point.y)
		var x = idx % xsz - 1 // -1 because of header row/column
		var y = Math.floor(idx/xsz) - 1
		console.debug(x,y,xsz, ysz, idx, point.x,point.y)
		return { "x" : x, "y" : y}
	}

}
