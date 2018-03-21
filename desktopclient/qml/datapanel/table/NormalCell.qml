import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global

Rectangle {
    function cellColor(columnIndex, rowIndex, selected){
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

	id : rectangle
    width : defaultWidth(styleData.column)
//    height : 18
    color : cellColor( styleData.column,styleData.row, (styleData.selected || table.isColumnSelected(styleData.column)))
	property bool editMode: false

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
					txt.forceActiveFocus()
				}
				onDoubleClicked: {
					editMode = true
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
//					if (table.isNumericalColumn(styleData.column)) table.setData(table.createIndex(styleData.row, styleData.column), parseDouble(edit.item.text))
				table.setData(table.index(styleData.row, styleData.column), text, styleData.column + 257)
				editMode = false
			}

			focus: true
		}
	}

	Loader {
		id:  editLoader
        anchors.fill: parent
        anchors.margins: 2
		focus : true

		sourceComponent: (editMode ? edit : label)

	}
}
