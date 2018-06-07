import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global

TextFieldStyle {
    id : textstyle
    textColor: "red"
    background: Rectangle {
        radius: 2
        border.color: Global.edgecolor
        border.width: 1
    }
}