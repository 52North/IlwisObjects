import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../creators" as Creators

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
			store()
        }
    }
    color : uicontext.lightestColor

	Column {
		anchors.fill : parent
		spacing : 4
		Text {
			width : parent.width
			height : 20
			text : getProperty("controlpointransformation")
		}
		Creators.MasterSlaveFields {
		}
		Creators.TiePointsTable {
			id : tiepointstable
			height : 280
		}

	}
	function georefUrl(){
		return rawUrl
	}

	function allDataSet() {
		return propertyForm.editable	
	}
}