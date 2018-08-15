import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import TabModel 1.0
import ModelRegistry 1.0
import "./propertyeditors" as LayerManagement
import "../../controls" as Controls
import "../../Global.js" as Global
import "../../pubsub.js" as PubSub
//import GeoDrawer 1.0

Item {
    id : toprow
    width : parent.width
    height : 35
    property var bid
    property var manager
    property alias panButton : maptools.panButton
    property alias zoomoutButton : maptools.zoomoutButton
    property alias zoominButton : maptools.zoominButton

    Controls.LayerExtentsToolbar{
        id : maptools
        width : parent.width
        height : parent.height

    }
    LocationBar{
        id : lb
    }
}