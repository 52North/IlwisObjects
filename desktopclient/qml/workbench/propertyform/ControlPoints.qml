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
        }
    }
    color : uicontext.lightestColor

	Creators.TiePointsTable {
		height : 280
	}

	function georefUrl(){
		return rawUrl
	}

	function allDataSet() {
		return propertyForm.editable	
	}
}