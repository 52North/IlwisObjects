import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../Global.js" as Global

Row {
    property alias checked : cb.checked
    property alias labelText : label.text
    property int labelWidth : 100
    property alias style : cb.style
    property alias exclusiveGroup : cb.exclusiveGroup
    property alias pressed : cb.pressed

    height : Global.rowHeight
    width : parent.width

	CheckBox { id : cb;  x : labelWidth}
    Text { id : label; text : label.text; font.bold: true; elide : Text.ElideRight}
}

