import QtQuick 2.2
import QtQuick.Controls 1.5 
import QtQuick.Controls 2.0 as QC20
import QtQml.Models 2.2
import UIContextModel 1.0
import LayerManager 1.0
import LayerModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

DropArea {
	width : 250
	height : 22
    property var img

    Row {
        id : layerline
	    width : 250
	    height : 22
	    spacing : 3

	    function cactive() {
		    var nodeid = styleData.value.nodeid
		    var layer = layerview.manager.findLayer(nodeid)
		    return layer.active
	    }
	    CheckBox{
		    y : 2
		    id : visibilityCheck
		    width : 17
		    height: 17
		    checked : layerline.cactive() // styleData.value.active
		    opacity : 0.6
		    visible : styleData.value && styleData.value.nodeid != 0

		    onCheckedChanged : {
			    var nodeid = styleData.value.nodeid
			    var layer = layerview.manager.findLayer(nodeid)
			    layer.vproperty("active", visibilityCheck.checked)
		    }


	    }
	    Image {
		    y : 2
		    id : icon
			    width : 16; height :16
		    source : styleData.value.icon ? uicontext.ilwisFolder + "/qml/images/" + styleData.value.icon : ""
		    fillMode: Image.PreserveAspectFit
		    opacity : 0.5
	    }						 
        Text {
		    y : 2
            elide: styleData.elideMode
            text: styleData.value ? styleData.value.name : ""
		    font.bold : styleData.value ? styleData.value.readonly : false
            height : 18
            width : 150
        } 

    }
    MouseArea{
        id : mouseArea
        anchors.fill : parent
        drag.target: img
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        onPressed : {
                    layerline.opacity = 0.5
                    img = Qt.createQmlObject('import QtQuick 2.0; Image{
                    id : image
                    width : 20; height : 20
                    source : styleData.value.icon ? uicontext.ilwisFolder + "/qml/images/" + styleData.value.icon : ""
                    fillMode: Image.PreserveAspectFit

                    Drag.keys: "layer"
                    Drag.active: mouseArea.drag.active
                    Drag.hotSpot.x: 10
                    Drag.hotSpot.y: 10
                    opacity : Drag.active / 2

                    states: State {
                        when: mouseArea.drag.active
                        ParentChange { target: img; parent: root }
                        AnchorChanges { target: img; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                    }
                }', mouseArea, "dynamicImage");
        }
    }
    onDropped : {
        layerline.opacity = 1
        if(!styleData.value.parenthasfixedstructure){
		    var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
		    console.debug("dropped2",styleData.value.nodeid, drag.source.ilwisobjectid, resource.name)
            tree.dropHandled = true
            var nodeid = styleData.value.nodeid
		    var cmd = "adddrawer(" + manager.viewid + ",\"\"," + resource.url + "," + resource.typeName + ",true," + resource.name + "," + nodeid + ")"
            layerview.manager.addCommand(cmd)
            setModel()
            layerview.manager.refresh()
        }
	}

    onEntered : {
        if(!styleData.value.parenthasfixedstructure) // if the parent is readonly no layers can be added to the childeren
            tree.hoveredRow = styleData.row
    }
    onExited : {
       tree.hoveredRow = -1
    }
}