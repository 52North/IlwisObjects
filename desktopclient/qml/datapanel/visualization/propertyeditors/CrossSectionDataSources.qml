import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../Controls" as Controls
import "../../.." as Base

Column { 
    anchors.margins : 5
    anchors.fill : parent
    spacing : 3
    Controls.TextEditLabelPair{
        id : datas
        width : parent.width -10
        height : 20
        labelWidth : 100
        labelText : qsTr("Data source")
		content : editor.dataSource ? editor.dataSource.source : ""
		onContentChanged : {
                editor.addDataSource(ilwisobjectid)
                bands.model = editor.bands	
		}
    }
    Text {
        text : qsTr("Bands") 
        width : parent.width
        height : 20
    }
    Rectangle{
        height : parent.height - datas.height - 25
        width : parent.width
        border.width: 1
        border.color: Global.edgecolor
        radius : 5

        ScrollView {
            anchors.fill: parent
            ListView {
                id : bands
                anchors.fill : parent
				model : editor.bands

                delegate : Row {
                    width : parent.width
                    height : 20
                    CheckBox {
                        height : 20
                        width : 20
                        checked : modelData.active
                        style : Base.CheckBoxStyle1{}
                        onClicked : {
                            editor.setActive(index, checked)
							var chartModel = models.model(modelid)
							if ( chartModel){
								for(var i=0; i < editor.pinCount; ++i){
									var expr = "updatechartseries(" + modelid + ","+ editor.tableUrl + "," + editor.pinDataColumn('bands')+ "," + editor.pinDataColumn(i+1) + ")"
									layerview.activeLayerManager().addCommand(expr);
								}
							}
						}
                    } 
                    Text {
                        text : modelData.name
                        width : parent.width -20
                        height : 18
                        y : 5
                    }
                }
            }
        } 
    }     
}