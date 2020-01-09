import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../.." as Base

Item {
    width: 100
    height: 62
    property var editor
	property var modelid : null

    Button{
        id : usesInfo
        x : 5
        y : 5
        text: "Open Histogram"
		width : 100
		height : 22

		onClicked : {
			  var mid = editor.visualAttribute.layer.modelId()
			  console.debug("kkkkk", editor.visualAttribute, editor.visualAttribute.layer, mid)
			  var createInfo = {type : "chart", url : editor.tableUrl, ctype : 'line', name : editor.editorName , xaxis : 'min', yaxis :'histogram', zaxis : '', extraparameters : 'specialtype=histogram|linkedid=' + mid}
              modelid = objectcreator.createObject(createInfo)
			  editor.chartModelId = modelid
              var filter = "itemid=" + modelid
              bigthing.newCatalog(filter, "chart", "","other")
		}
    }

}

