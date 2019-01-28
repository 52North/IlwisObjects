import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

Row {
    id : domainForm
	anchors.fill : parent
	property var domain
	spacing : 30
	Column {
	    height : parent.height
		width : 300
		spacing : 4
		x : 4
		Controls.TextEditLabelPair {
			id :multisr
			labelText : qsTr("Classifier Domain")
			labelWidth : 100
			checkFunction : testDrop
			width : 300
		}

		TableView {
			width : 300
			height : 180
			id : items
			model : domain ? domain.domainitems : null
			TableViewColumn{
				title : qsTr("Item name")
				role : "name"
				width : 180
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
				height : 15
				color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
			}
		}
	}

	Column {
		id : act
	    y : 20
	    width : 300
		height : parent.height - 20
		spacing : 10
		property var minv : 0
		property var maxv : 0
		visible : domain != null
		Row {
		    width : 390
			height : 20
			spacing : 5			
			Text {
				text :  qsTr("Spectral Distance")
				width : 100
				height : 20
			}
			Text {
				width : 45
				height : 20
				text : act.minv
			}
			QC2.Slider {
				id : spDistance
				width : 200
				height : 20
				stepSize : 1

				handle: Rectangle {
					x: spDistance.leftPadding + spDistance.visualPosition * (spDistance.availableWidth - width)
					y: spDistance.topPadding + spDistance.availableHeight / 2 - height / 2
					implicitWidth: 29
					implicitHeight: 24
					radius: 7
					color: spDistance.pressed ? "#f0f0f0" : "#f6f6f6"
					border.color: "#bdbebf"
					Text {
						text : spDistance.value
						anchors.centerIn : parent
						width : parent.width
						height : parent.height
						horizontalAlignment : Text.AlignHCenter
						verticalAlignment : Text.AlignVCenter
						font.pointSize : 8
					}
				}
				onMoved : {
					analisysView.view().setSpectralDistance(value)
				}
			}
			Text {
				width : 45
				height : 20
				text : act.maxv
			}
		}
		Button {
		    visible : items.currentRow != -1 && domain != null
			text : qsTr("Add Selected Pixels to\n selected class")
			width : 200
			height: 40
		}
	}
	 function testDrop(id){
        if (!id)
            return false
		var ok = false
        var obj = mastercatalog.id2object(id, domainForm)
        if ( obj) {
			if (obj.typeName === "itemdomain"){
				var rng = ccRaster.getProperty("numericrange")
				var vmin = rng.split("|")[0]
				var vmax = rng.split("|")[1]
				act.maxv = Math.ceil((vmax - vmin) / 4)
				act.minv = 0
			    spDistance.to = act.maxv
				spDistance.from = act.minv
			    domain = obj;
				ok = true
			}
        }
        return ok
    }
}