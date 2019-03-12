import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

SplitView {
    orientation: Qt.Horizontal

    Item{
        width : parent.width / 2
        height : parent.height
    }

    Item{
        width : parent.width / 2
        height : parent.height
    }
}