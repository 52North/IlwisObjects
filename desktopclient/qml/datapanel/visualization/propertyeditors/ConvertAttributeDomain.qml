import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../.." as Base

Column {
    height: 80
    property var editor
    property string currentScheme : ""
    
    Text {
        width : parent.width
        height : 35
        text : qsTr("Color Schemes")
    }
    ComboBox{
            id : colorscheme
            width : Math.min(parent.width * 0.85, 250)
            height : 20
            model :  editor.colorSchemes

            onCurrentIndexChanged : {
                if ( currentScheme != colorscheme.currentText){
                    currentScheme = colorscheme.currentText
                    editor.execute(currentScheme,layerview.manager.viewid)
                }
            }
    }
}


