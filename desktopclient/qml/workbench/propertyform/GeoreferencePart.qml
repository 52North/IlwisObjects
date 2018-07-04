import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    width: parent.width
    height: 70
    property int lineheight : 19
    color : uicontext.lightestColor
    function storeData() {
        if ( propertyForm.editable){
            setAttribute("georeference", georef.content,"")
        }
    }

    Column {
        width : parent.width
        height : 60

        Controls.FilteredTextEditLabelPair{
            id : line1
            labelWidth: 120
            labelText: qsTr("Georeference")
            filterImage: "../images/georeference.png"
            filterType: "georeference"
            width : parent.width
            content: isAnonymous ? "internal" : geoReferenceName
            readOnly: !propertyForm.editable
        }

        Controls.TextEditLabelPair{
            labelText :qsTr("Raster Size")
            labelWidth : 120
            width : parent.width - 20
            height : 20
            content : dimensions
            readOnly : true
        }
        Controls.TextEditLabelPair{
            labelText :qsTr("Pixel Size")
            labelWidth : 120
            width : parent.width - 20
            height : 20
            content : getProperty("pixelsize")
            readOnly : true
        }
    }
}
