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
    

    Row {
        id : layerline
	    width : 250
	    height : 22
	    spacing : 3
        opacity : styleData.row == tree.rowBeingMoved ? 0.5 : 1

	    function cactive(data) {
		    var nodeid = data.nodeid
		    var layer = layerview.activeLayerManager().findLayer(nodeid)
		    return layer.active
	    }
	    CheckBox{
		    y : 2
		    id : visibilityCheck
		    width : 17
		    height: 17
		    checked :  styleData.value && styleData.value.active  // layerline.cactive(styleData.value)
		    opacity : 0.6
		    visible : styleData.value && styleData.value.nodeid != 0

		    onCheckedChanged : {
                var nodeid = styleData.value.nodeid
			    var layer = layerview.activeLayerManager().findLayer(nodeid)
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
	    Image {
		    y : 2
		    id : readonly
			    width : 10; height :10
		    source : uicontext.ilwisFolder + "/qml/images/readonly.png"
		    fillMode: Image.PreserveAspectFit
		    opacity : 0.5
             visible : styleData.value ? styleData.value.parenthasfixedstructure : false

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
        width : parent.width - visibilityCheck.width
        height : parent.height
        x :   visibilityCheck.width + 3
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        propagateComposedEvents: true
        property variant img
        drag.target: img

        onPressed : {
            tree.selection.setCurrentIndex(styleData.index, ItemSelectionModel.ClearAndSelect)
            setTreeIndex(styleData.index)
        }
        onReleased : {
            if ( img){
                img.Drag.drop()
                img.parent = mouseArea
                img.anchors.fill = mouseArea
                img.destroy();
            }            
        }


        onPressAndHold : {
                    if (!styleData.value.parenthasfixedstructure){ // no movements of layers from a fixed layer (e.g. featurecoverage)
                        img = Qt.createQmlObject('import QtQuick 2.0; Image{
                        id : image
                        width : 20; height : 20
                        source : styleData.value.icon ? uicontext.ilwisFolder + "/qml/images/" + styleData.value.icon : ""
                        fillMode: Image.PreserveAspectFit
                        property string nodeid : styleData.value.nodeid

                        Drag.keys: "layer"
                        Drag.active: mouseArea.drag.active
                        Drag.hotSpot.x: 10
                        Drag.hotSpot.y: 10
                        opacity : Drag.active / 2

                        states: State {
                            when: mouseArea.drag.active
                            ParentChange { target: mouseArea.img; parent: root }
                            AnchorChanges { target: mouseArea.img; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                        }
                    }', mouseArea, "dynamicImage");

                    tree.rowBeingMoved = styleData.row
                }
        }
    }
    onDropped : {
        if(!styleData.value.parenthasfixedstructure){
            if ( typeof drag.source.ilwisobjectid !== 'undefined'){
		        var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid, dropArea)
                tree.dropHandled = true
                var nodeid = styleData.value.nodeid
		        var cmd = "adddrawer(" + activeLayerManager().viewid + ",\"\"," + resource.url + "," + resource.typeName + ",true," + resource.name + "," + nodeid + ")"
                layerview.activeLayerManager().addCommand(cmd)
                setModel()
                layerview.activeLayerManager().refresh()
            }else if(typeof drag.source.nodeid !== 'undefined'){
                if ( styleData.value.nodeid != 0 ){ // nodeid = 0 is foreground layer, you can't insert layers before that
                    layerview.activeLayerManager().move(drag.source.nodeid, styleData.index)  
                  } 
            }
            tree.hoveredRow = -1
        }

        tree.rowBeingMoved = -1
	}

    onEntered : {
        if(!styleData.value.parenthasfixedstructure && styleData.value.nodeid != 0) {// if the parent is readonly no layers can be added to the childeren, neither are inserts possible before the foreground
            tree.hoveredRow = styleData.row
        }
    }
    onExited : {
       tree.hoveredRow = -1
    }
}