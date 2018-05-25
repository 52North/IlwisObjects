import QtQuick 2.0

MouseArea {
    id : dragArea
    anchors { left: parent.left; right: parent.right }
    height: content.height
    hoverEnabled: true
    x : 10 + modelData.level * 20
	property var droppedFunction

    property bool held: false

    drag.target: held ? content : undefined
    drag.axis: Drag.YAxis

    onPressAndHold: held = true
    onReleased: {

        if (held){
            tree.changeOrder(index, tree.dropIndex)


        }
        held = false;

    }

    onClicked: {
        tree.adapt(modelData.index)


    }
    DropArea {
        anchors { fill: parent}

        onEntered: {
            tree.dropIndex = index
        }
		onDropped: {
			if ( droppedFunction && modelData.extensible)
				droppedFunction(drag.source.ilwisobjectid)
        }
    }
    Rectangle {
        id : content
        width: dragArea.width; height: 20

        color: dragArea.held ? "lightsteelblue" : "white"
        Behavior on color { ColorAnimation { duration: 100 } }

        Drag.active: dragArea.held
        Drag.source: dragArea
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        states: State {
            when: dragArea.held

            ParentChange { target: content; parent: tree }
            AnchorChanges {
                target: content
                anchors { horizontalCenter: undefined; verticalCenter: undefined }
            }
        }

    }
}
