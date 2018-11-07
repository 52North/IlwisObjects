import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "./propertyeditors" as DisplayOptions
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Row {
    id : header
    height : 24
    anchors.verticalCenter: parent.verticalCenter
	anchors.right : parent.right
	anchors.rightMargin : 8
	spacing : 2

    TextField{
        id : nativeCoords
        width : header.width * 0.5
        height : 18
        readOnly: true
        anchors.verticalCenter: parent.verticalCenter
        text : manager ? manager.rootLayer.currentLatLon : ""
    }

    TextField{
        id : otherCoords
        width : header.width * 0.5 
        height : 18
        readOnly: true
        anchors.verticalCenter: parent.verticalCenter
        text : manager ? manager.rootLayer.currentCoordinate : ""
    }
}

