import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../.." as Base

Rectangle {
    height: parent ? parent.height - 10 : 0
    property var editor


    Column {
        id : col
        y : 8
        width : Math.min(parent.width * 0.95, 250)
        height : childrenRect.height + 10
        spacing : 4

        Text {
            text : qsTr("Domain type")
            width : 18
        }
        ComboBox{
            id : targets
            width : Math.min(parent.width * 0.95, 250)
            height : 20
            model :  editor.allowedDomains()
            Component.onCompleted: {
                currentIndex = editor.domainTypeIndex()
            }
        }
        Text {
            text : qsTr("Color scheme")
			height : 20
            width : 18
        }

        ComboBox{
            id : colorscheme
            width : Math.min(parent.width * 0.95, 250)
            height : 20
            model :  editor.colorSchemes
        }
        Column {
            height : 45
            width : Math.min(parent.width , 250)
            CheckBox{
                id : check
                width : parent.width
                height : 20
                checked : false
                text : qsTr("Domain name")
				style: Base.CheckBoxStyle1{}
            }
            TextField{
                id : domainName
                width :parent.width
                height : 20
                enabled : check.checked
                opacity : check.checked ? 1 : 0.25
            }
        }

    }
    Button{
        width : 80
        height : 22
        text : qsTr("Apply")
        anchors.right : col.right
        //anchors.rightMargin:
        anchors.top : col.bottom
        onClicked: {
            var attrIndex = attributeListColumn.currentIndex
			console.debug("aaaa", layerview.manager.viewid)
            if (editor.execute(targets.currentText, domainName.text, colorscheme.currentText,layerview.manager.viewid)){
                attributeListColumn.currentIndex = attrIndex
            }
        }
    }
}


