import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../.." as Base

Item {
    id : crosssectiontool 
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor
    property var modelid : null
    property int contineousPin : -1
    property bool contineousMode : false
	clip : true

    TabView {
        anchors.fill : parent
   Tab {
            id : tab1
            title: "Pin Manager"
            CrossSectionPinManager{
                id : pinmanager
            }
    }
    Tab {
            title: "Data Source(s)"
            CrossSectionDataSources{}
        }
    }

    function handleMousePressed(mx,my){
        if (crosssectiontool.contineousPin != -1){ 
            contineousMode = true
            editor.changeCoords(contineousPin, mx, my, true)
            if ( modelid == null){
                var createInfo = {type : "chart", url : editor.tableUrl, ctype : 'points', name : editor.editorName , xaxis : editor.pinDataColumn(0), yaxis :'continuous_pin', zaxis : ''}
                modelid = objectcreator.createObject(createInfo)
				editor.chartModelId = modelid
                var filter = "itemid=" + modelid
                bigthing.newCatalog(filter, "chart", "","other")
            }else {
                var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn(0)+ ",continuous_pin)"
                layerview.activeLayerManager().addCommand(expr);
            }
        }/*else{
            handleMouseClick(mx,my)
        }*/

    }

    function  handleMouseMoved(mx,my){
        if ( contineousMode == true && modelid != null && contineousPin != -1) {
                editor.changeCoords(contineousPin, mx, my, true)
                var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn(0)+ ",continuous_pin)"
                layerview.activeLayerManager().addCommand(expr);        
        }
    }

    function handleMouseReleased(mx,my){
        contineousMode = false;

    }

	function reopenChart(){
		var pc = editor.pinCount
		var createInfo = {type : "chart", url : editor.tableUrl, ctype : 'points', name : editor.editorName , xaxis : editor.pinDataColumn('bands'), yaxis : editor.pinDataColumn(1), zaxis : ''}
		modelid = objectcreator.createObject(createInfo)
		editor.chartModelId = modelid
		var filter = "itemid=" + modelid
		bigthing.newCatalog(filter, "chart", "",layerview.tabmodel.side == "left" ? "right" : "left")
		for(var i=0; i < editor.pinCount; ++i){
			var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(i+1) + ")"
			layerview.activeLayerManager().addCommand(expr);
		}
	}

    function handleMouseClick(mx,my){
        if (!editor.contineousMode()){
            if ( tab1.item.selectedRow >= 0){
                editor.changeCoords(tab1.item.selectedRow, mx, my, true)
				modelid = editor.chartModelId
                if ( modelid == 0){
                    var createInfo = {type : "chart", url : editor.tableUrl, ctype : 'points', name : editor.editorName , xaxis : editor.pinDataColumn(0), yaxis : editor.pinDataColumn(1), zaxis : ''}
                    modelid = objectcreator.createObject(createInfo)
					editor.chartModelId = modelid
                    var filter = "itemid=" + modelid
                    bigthing.newCatalog(filter, "chart", "","other")
                 }else {
				    var chartModel = models.model(modelid)
					if ( chartModel){
						var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(tab1.item.selectedRow + 1) + ")"
						layerview.activeLayerManager().addCommand(expr);
					}else {
						reopenChart()
					}
                 }
             }
        }
    }
}

