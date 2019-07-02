import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls


Rectangle {
    width: parent.width - 5
    height: parent ? parent.height - 10 : 0
    property var operation

	  Component {
        id : layerDrawer
        Controls.DummySpatial{
            id : lyrview
            anchors.fill: parent

            property var resource
            property var controller
			postRenderCallBack : frame.finalizeDraw
			
            function setResource(object, res){
				if ( !lyrview.layermanager)
					lyrview.layermanager = models.createLayerManager(layerDrawer,layerDrawer)
				else{
				    lyrview.reset()
					lyrview.layermanager.reset()

				}
                var cmd = "adddrawer(" + lyrview.layermanager.viewid + ",\"\",\"itemid=" + res.id + "\"," + res.typeName + ",true,\"\")"
                lyrview.addCommand(cmd)
                lyrview.layermanager.rootLayer.vproperty("griddraweractive", false)
				lyrview.initSizes()
            }
        }
    }

	Column {
        id : previews
        width : 240
        anchors.leftMargin: 3
        height : parent.height - 5
        spacing : 6
        Rectangle {
            id : header2
            width : previews.width
            height : 18
            color : uicontext.actionHeaderColor
            Text{
                text : qsTr("Previews")
                font.bold: true
                x : 3
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id : frame
            objectName : uicontext.uniqueName()
            property var resources : []
            property int currentMap : 0

            function setResource(){
                if ( frame.resources.length > 0 && frame.currentMap < frame.resources.length){
                    //if(!imageLoader.sourceComponent)
						imageLoader.sourceComponent = layerDrawer
                    imageLoader.item.setResource(frame, frame.resources[currentMap])
                }
            }

            function makeResourceList() {
                var list = mastercatalog.selectedData
                frame.resources = []
                currentMap = 0
                progress.maximumValue = list.length
                for(var i=0; i < list.length; ++i){
                    var resource = list[i]
                    if ( resource.typeName === "rastercoverage" ||
                            resource.typeName === "featurecoverage" ||
                            resource.typeName === "pointcoverage" ||
                            resource.typeName === "linecoverage" ||
                            resource.typeName === "polygoncoverage")
                        frame.resources.push(resource)
                }
                setResource()


            }

            function finalizeDraw() {
				if ( frame.resources.length > 0 && frame.currentMap < frame.resources.length){
					var path = catalogViews.currentCatalog.specialFolder("thumbs");
					var res = frame.resources[currentMap]
					var thumbname = res.containerFile + res.name + ".png"
					var thumbPath = path + "/" + thumbname
					frame.grabToImage(function(result) {
						  result.saveToFile(thumbPath);
					});
					if ( frame.resources[currentMap]){
						frame.resources[currentMap].unload()
						frame.resources[currentMap].imagePath = "file:///" + thumbPath
					}
					++currentMap
					progress.value = currentMap
					setResource()
				}
            }

            anchors.left : parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            height : 150
            border .width: 1
            border.color: "lightgrey"

            Loader{
                id : imageLoader
                anchors.fill: parent
            }
            ProgressBar{
                id : progress
                height : frame.height
                width : 15
                anchors.left: frame.right
                anchors.top : frame.top
                orientation: Qt.Vertical
                minimumValue: 0
                maximumValue: 0
            }

        }
		Button {
			height : 20
			width : 120
			text : qsTr("Generate Thumbnails")
            onClicked: {
                frame.makeResourceList()
				mastercatalog.currentCatalog.scanContainer(true,true)
            }
		}
	}
}