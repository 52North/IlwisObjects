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
    Controls.ComboxLabelPair{
        id : datas
        width : parent.width -10
        height : 20
        labelWidth : 100
        labelText : qsTr("Data source(s)")
        itemModel :  editor.dataSources
        role : "source"
        DropArea {
            id: cbDropArea
            anchors.fill: parent
            enabled: true
            onDropped: {
                editor.addDataSource(drag.source.ilwisobjectid)
                datas.currentIndex = datas.itemModel.length - 1
                bands.model = editor.band(datas.currentIndex)
            }
        }
        onCurrentIndexChanged : {
            if ( itemModel) {
                bands.model = editor.band(currentIndex)
            }
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

                delegate : Row {
                    width : parent.width
                    height : 20
                    CheckBox {
                        height : 20
                        width : 20
                        checked : modelData.active
                        style : Base.CheckBoxStyle1{}
                        onCheckedChanged : {
                            editor.setActive(datas.currentIndex, bands.index, checked)
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