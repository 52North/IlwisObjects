import QtQuick 2.0
import QtQuick.Controls 1.0

Row {
    Button {
        id : editBut
        text : qsTr("Edit")
        checkable: true
        checked : false
        onClicked: {
      		if ( checked){
				propertyForm.storeData()
			}
            checked = !checked
            propertyForm.editable = !propertyForm.editable
            mastercatalog.metadataEditMode = (text === qsTr("Edit"))
            text = (text === qsTr("Edit")) ? qsTr("Store") : qsTr("Edit")
	
        }
    }
    Button {
        id : applyBut
        text : qsTr("Cancel")
        visible : propertyForm.editable
        onClicked: {
			mastercatalog.metadataEditMode = false
			propertyForm.editable = false
			//propertyFormContainer.reset()
        }
    }
}

