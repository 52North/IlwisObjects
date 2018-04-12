import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4
import "../../Global.js" as Global

Rectangle {
	id : rectangle

	property bool editMode: false

	// Extract the domain items (only for ItemDomain)
    function modelFromItems(rangedef) {
        if ( rangedef === "")
            return "";

        var index = rangedef.indexOf(":");
        var itemstring = rangedef.substring(index + 1);
        var items = itemstring.split("|");

		itemDomainValueList.clear();
		for (var v in items) {
			itemDomainValueList.append({text:items[v]})
		}

		return itemDomainValueList;
    }

	// activateComponent selects the appropriate editor / label
	// It also fills the model list in case of item domains
	function activateComponent(columnNumber) {
		var col = table.columns[columnNumber];

		if (editMode) {
			if (col.attributeDomainType == "Item Domain") {
				var list = modelFromItems(col.defaultRangeDefinition);
				return editCombo;
			}
			else {
				if (col.attributeDomainType == "Value Domain") {
					var range = col.actualRangeDefintion;
					console.log("Range="+range)
				}
				return edit;
			}
		}
		else {
			// non-editable value (as strings)
			return label;
		}
	}

    function cellColor(columnIndex, rowIndex, selected) {
	    if (rowIndex == -1 || columnIndex == -1)
			return "#fff"
        if ( selected)
            return Global.selectedColor
        var rowBand = (rowIndex % 10) > 4
        var colBand = (columnIndex) % 2 == 1
        if ( rowBand && colBand)
            return "#E4EBE5"
        if ( rowBand)
            return "#EEF2EE"
        if ( colBand)
            return "#F8FAF8"
        return "#fff"
    }

    width : defaultWidth(styleData.column)
    height : 22
    color : cellColor( styleData.column,styleData.row, (styleData.selected || table.isColumnSelected(styleData.column)))

	Loader {
		id:  editLoader
        anchors.fill: parent
        anchors.margins: 2
		focus : true

		sourceComponent : activateComponent(styleData.column)	// activate component also reads all domain items

	}

	ListModel {
		id: itemDomainValueList
	}

	Component {
		id: editCombo

		ComboBox {
			id: combo

			editable: false
			focus: true
			model: itemDomainValueList
			onActivated: {
				// to handle user changes in not editable mode of combobox; this is the default
				table.setData(table.index(styleData.row, styleData.column), model.get(index).text, styleData.column + 257)
				editMode = false;
			}
			onAccepted: {
				// to handle ENTER to accept change in editable mode of comcobox
				table.setData(table.index(styleData.row, styleData.column), combo.currentText, styleData.column + 257)
				editMode = false
			}

			onActiveFocusChanged: {
				// Ignore any current change (in non-editable mode of combobox)
                if (!activeFocus) {
                    editMode = false;
                }
            }

			Keys.onEscapePressed: {
				// Ignore any current change (in editable mode of combobox)
				editMode = false
				event.accepted = true;
			}

			Component.onCompleted: {
				// Set index to match current value
				var index = combo.find(styleData.value);
				if (index != -1) combo.currentIndex = index;
			}

		}
	}

	Component {
		id: label

		Text {
			id : txt
			width : rectangle.width - 5
			height : rectangle.height
			text: styleData.value ? styleData.value : (table.isNumericalColumn(styleData.column) ? 0 : "")
			color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? "#2121A3" : "black"
			elide: Text.ElideMiddle
			horizontalAlignment: table.isNumericalColumn(styleData.column) ? Text.AlignRight : Text.AlignLeft
			x : 2

			MouseArea {
				id: mouseArea
				anchors.fill: parent
				hoverEnabled: true
				onClicked : {
					txt.forceActiveFocus();
				}
				onDoubleClicked: {
					editMode = true;
				}
			}
		}
    }

	Component {
		id: edit

		TextField {
			id:inplace
	        anchors.fill: parent

            text: styleData.value

			horizontalAlignment: Text.AlignLeft
            style: TextFieldStyle {
                background: Rectangle {
                    width : parent.width
                    height: parent.height
                    border.color: "black"
                    color : "white"
                }
            }

			Component.onCompleted: {
				inplace.selectAll()
				inplace.forceActiveFocus()
			}

			Keys.onEscapePressed: {
				editMode = false
				event.accepted = true;
			}

			onEditingFinished: {
				editMode = false
			}

			onAccepted: {
				// 257 === Qt::UserRole + 1
				if  (table.isNumericalColumn(styleData.column)) {
					table.setData(table.index(styleData.row, styleData.column), parseFloat(text), styleData.column + 257);
				} else {
					table.setData(table.index(styleData.row, styleData.column), text, styleData.column + 257);
				}
				editMode = false
			}

			focus: true
		}
	}

}
