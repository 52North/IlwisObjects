import QtQuick 2.0
import QtQuick.Controls 1.0

Row {
    Button {
        id : editBut
        text : isEditable ? qsTr("Store") : qsTr("Edit")
        checkable: true
        checked : false
        onClicked: {
      		if ( checked){
				propertyForm.storeData()
			}
            checked = !checked
            isEditable = !isEditable
            mastercatalog.metadataEditMode = isEditable
        }
    }
    Button {
        id : applyBut
        text : qsTr("Cancel")
        visible : isEditable
        onClicked: {
			mastercatalog.metadataEditMode = false
			isEditable = false
        }
    }
}

