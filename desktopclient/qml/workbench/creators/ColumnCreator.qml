import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
    id : columnCreation
    height : 180
    width : parent ? parent.width : 0
    spacing : 4
    property var numericColumn : false

    Component {
        id : minmaxrange
        Column {
            width : parent.width
            height : 50
            Controls.TextEditLabelPair{
                id : columnminvalue
                labelText: qsTr("Minimum Value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^-?\d*(\.\d*)?$/
            }

            Controls.TextEditLabelPair{
                id : columnmaxvalue
                labelText: qsTr("Maximum Value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^-?\d*(\.\d*)?$/
            }
        }
    }

    Component {
        id : itemrange
        Controls.ItemDomainSelector{
        }
    }

    Button{
        anchors.right: parent.right
        anchors.rightMargin: 2
        width : 80
        height : 22
        text : "New Column"
        onClicked: {
            newColumn.enabled = true
        }
    }
    Text {
        width : parent.width
        height : 20
        text : "Column Definition"
        opacity : newColumn.enabled ? 1 : 0
    }

    Column {
        id :newColumn
        width : parent.width
        height : 140
        enabled : false
        opacity : enabled ? 1.0 : 0
        spacing: 3
        Controls.TextEditLabelPair{
            id : columnname
            labelText: qsTr("Name")
            labelWidth: 100
            width : parent.width
        }
        Controls.TextEditLabelPair{
            id : columndomain
            labelText: qsTr("Domain")
            labelWidth: 100
            width : parent.width
            keys : ['domain','numericdomain','itemdomain','colordomain','textdomain']

            onContentChanged : {
               if ( ilwisobjectid){
                    var domain = mastercatalog.id2object(ilwisobjectid, columnCreation)
                  
                    if ( domain){
                        if ( domain.typeName === "itemdomain" ){
                            rangeDefinition.sourceComponent = itemrange
                            rangeDefinition.item.domainModel = domain
                        }else if (domain.typeName === "numericdomain") {
                            rangeDefinition.sourceComponent = minmaxrange
                        }
                    }
                }else
                    domain = null
            }
        }
        Rectangle {
            width : parent.width
            height : 68
            border.width : 1
            border.color : Global.edgecolor
            Loader {
            id : rangeDefinition

            }
        }


        Button{
            anchors.right: parent.right
            anchors.rightMargin: 2
            width : 80
            height : 22
            text : "Add Column"
            onClicked: {
                newColumn.enabled = false
                var items = []
                if ( numericColumn){
                    var item = ({name: columnname.content, minvalue : rangeDefinition.item.columnminvalue.content, maxvalue : rangeDefinition.item.columnmaxvalue.content,domain: columndomain.content})
                    addColumn(item, numericColumn)
                }else {
                    var item = {name: columnname.content, domain: columndomain.content}
                    addColumn(item, numericColumn)
                }
            }
        }
    }
}



