import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Column {
    height: parent ? parent.height - 10 : 0
    property var editor
    x : 5

    Text{
         id : label
         text : qsTr("Grid Color")
    }

   Controls.ColorPicker2{
        width : parent.width - 20 
        onSelectedColorChanged: {
           editor.lineColor = selectedColor
       }
       Component.onCompleted : {
           console.debug("cp width", width)
       }
   }
}

