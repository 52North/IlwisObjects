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
    height : 160
    width : parent ? parent.width : 0
    spacing : 4
    Button{
        anchors.right: parent.right
        anchors.rightMargin: 2
        width : 80
        height : 22
        text : "New Item"
        onClicked: {
            newItem.enabled = true
			dropItem.editorEnabled = true

        }
    }
    Text {
        width : parent.width
        height : 20
        text : "Numeric Interval"
        opacity : newItem.enabled ? 1 : 0
    }

    Column {
        id :newItem
        width : parent.width
        height : 100
        enabled : false
        opacity : enabled ? 1.0 : 0
        spacing: 3
        Controls.TextEditLabelPair{
            id : itemname
            labelText: qsTr("Name")
            labelWidth: 100
            width : parent.width
        }
        Controls.DateTimeDropDown{
			    id : itemminvalue
				width : 370
				labelText : qsTr("Begin Date/Time")
				labelWidth : 100
				z  : 100
				useWholeDays : dropItem.useWholeDays()
		}
        Controls.DateTimeDropDown{
			    id : itemmaxvalue
				width : 370
				labelText : qsTr("End Date/Time")
				labelWidth : 100
				z : 99
				useWholeDays : dropItem.useWholeDays()
		}

        Controls.TextEditLabelPair{
            id : itemcode
            labelText: qsTr("Code")
            labelWidth: 100
            width : parent.width
        }

        Controls.TextEditLabelPair{
            id : itemdesc
            labelText: qsTr("Description")
            labelWidth: 100
            width : parent.width
        }
        Button{
            anchors.right: parent.right
            anchors.rightMargin: 2
            width : 80
            height : 22
            text : "Add Item"
            onClicked: {
                newItem.enabled = false
                var items = []
                items.push({name: itemname.content, minvalue : itemminvalue.content, maxvalue : itemmaxvalue.content,code: itemcode.content, description : itemdesc.content})
                addDomainItems(items, false)
				dropItem.editorEnabled = false
            }
        }
    }
}



