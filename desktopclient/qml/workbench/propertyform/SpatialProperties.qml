import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
            csyProps.storeData()
            if ( rasterProps.item){
                rasterProps.item.storeData()
            }
        }
    }
        anchors.fill : parent
    color : uicontext.lightestColor
    CoordinateSystemProperties{
        id : csyProps
    }

    Loader {
        id : rasterProps
        width : parent.width
        source : typeName == "rastercoverage" ? "GeoreferencePart.qml" : ""
        anchors.top : csyProps.bottom
    }
}
