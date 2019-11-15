import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../Global.js" as Global

Rectangle {
    id:comboBox
    property alias selectedColor : colorPicker.lastSelectedColor
    property var initialColor
	property var closeCallBack
	  property int dropdownheight : 180

    onInitialColorChanged : {
		if ( initialColor != null) {
			choosenItemColor.color = initialColor ? initialColor : "white"
			chosenItemText.text = initialColor ? initialColor : "white"
			selectedColor = initialColor
		}
    }

	onSelectedColorChanged : {
		choosenItemColor.color = selectedColor
		chosenItemText.text = selectedColor
	}

    width : 235
    height : Global.rowHeight + dropDown.height
    z: 100;
    smooth:true;
    color : "transparent"
  

    Rectangle {
        id:chosenItem
        radius:2;
        width:parent.width;
        height:Global.rowHeight
        color: Global.alternatecolor3
        border.width: 1
        border.color: "#9494B8"
        smooth:true;
        Rectangle {
            id : choosenItemColor
            anchors.left: parent.left
            anchors.leftMargin: 5
            height: parent.height - 4
            width : 16
            color : selectedColor ? selectedColor : "white"
            y : 2
        }

        Text {
            width: parent.width-50; height: parent.height
            anchors.left: parent.left; anchors.leftMargin: 25;
            verticalAlignment: Text.AlignVCenter
            id:chosenItemText
            font.family: "Arial"
            font.pixelSize: 12;
            text : selectedColor ? selectedColor : ""
            smooth:true
        }

        Item {
            width: 20; height: 20
            anchors.right: parent.right
            Image {
                width: 16; height:16;
                anchors.centerIn: parent;
                source: "../images/down20.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                comboBox.state = comboBox.state==="dropDown"?"":"dropDown"
            }
        }
    }

    Rectangle {
        id:dropDown
        width:comboBox.width
        height:0
        clip:true
        radius:4
        anchors.top: chosenItem.bottom
        anchors.margins: 1
        color: uicontext.workbenchBGColor

        border.width: 1
        border.color: "#B0B0B0"

        Rectangle {

            id : colorPicker

            property int maxDisks : 7
            property int numberOfCircles : maxDisks
            property var currentDiskColor
            property var lastSelectedColor



            onNumberOfCirclesChanged: {
                diskcanvas.requestPaint()
            }
            y : 2
            x : 2
            width : Math.min(190, comboBox.width )
            height : dropdownheight  - 10
            ExclusiveGroup { id: colordbuttongroup }

            function drawSegment(ctx, cx,cy,cellsize,radius,angle1, angle2, clr){
                if ( radius <= 0)
                    return
                var ax = cx + radius * Math.cos(angle1);
                var ay = cy + radius * Math.sin(angle1);
                ctx.beginPath();
                ctx.strokeStyle =  clr
                ctx.moveTo(ax, ay)
                ctx.beginPath()
                ctx.lineWidth = cellsize;
                ctx.arc(cx, cy, radius, angle1, angle2)
                ctx.stroke()
            }

            function color2Hex(color) {
                if (color.substr(0, 1) === '#') {
                    return color;
                }
                var rgb = color.split( ',' ) ;
                var r=parseInt( rgb[0].substring(4) ) ; // skip rgb(
                var g=parseInt( rgb[1] ) ; // this is just g
                var b=parseInt( rgb[2] ) ;
                var clr = "#" + ("00" + r.toString(16)).slice(-2)  +
                        ("00" + g.toString(16)).slice(-2) +
                        ("00" + b.toString(16)).slice(-2)
                return clr;
            }

            function color(disk, circle, index){
                var code= ("00" + disk.toString(10)).slice(-2) +
                        ("00" + circle.toString(10)).slice(-2) +
                        ("00" + index.toString(10)).slice(-2)
                return uicontext.code2color(code)

            }

            Text {
                x : 2
                id : colorInfo
                height : 16
                width : 70
                text :  "white"
                anchors.top : parent.top
                anchors.topMargin: 4
            }
            Rectangle{
                id : lastSelected
                anchors.right : parent.right
                anchors.rightMargin: 3
                anchors.top : colorInfo.top
                width : 30
                height : colorInfo.height
                color : colorPicker.lastSelectedColor ? colorPicker.lastSelectedColor : "white"
                border.width: 1
                border.color: "grey"

            }
            Rectangle{
                id : currentColor
                anchors.right : lastSelected.left
                anchors.leftMargin: 3
                anchors.top : colorInfo.top
                width : 30
                height : colorInfo.height
                color : colorInfo.text
                border.width: 1
                border.color: "grey"

            }

            Row {
                anchors.top : colorInfo.bottom
                width : parent.width
                height : colorPicker.height - 5
                Column {
                    id : diskselection
                    property int currentIndex : 0
                    width : 30
                    height : parent.height
                    ExclusiveGroup { id: colordiskgroup }
                    Repeater {
                        model : colorPicker.maxDisks
                        RadioButton {
                            x : 2
                            width : 30 //parent.width / (colorPicker.maxDisks + 0.2)
                            height : 18
                            exclusiveGroup: colordiskgroup
                            text : index + 1
                            onClicked: {
                                colorPicker.numberOfCircles = colorPicker.maxDisks - index
                                diskselection.currentIndex = index
                            }
                            Component.onCompleted: {
                                checked = (index == 0)
                            }
                        }
                    }
                }
                Rectangle {
                    id : disk
                    property int diskRadius : Math.min(width/2, height/2 ) //Math.min(72, comboBox.width / 3)
                    property var radii : []
                    y : 2
                    width : parent.width - diskselection.width - 2
                    height : colorPicker.height - 15 //159

                    function getColor(mouseX, mouseY, clicked){
                        var dx = mouseX - disk.width / 2
                        var dy = mouseY - (disk.height / 2)
                        var r = Math.sqrt(dx*dx + dy*dy)
                        var cellsize = (disk.diskRadius / colorPicker.numberOfCircles)
                        var circle = cellsize > 0 ? Math.floor(r / (cellsize  )) : 0

                        var nc = 6 * circle
                        var tan = dx != 0 ? dy / dx : 100000.0
                        var an = Math.atan(tan) * 180/ Math.PI
                        var id = nc !== 0 ? Math.floor(nc * (an + 360/(nc *2)) / 360.0 ) : 0
                        if ( dx < 0 && dy >= 0){
                            id = nc / 4 + (nc/4 + id)
                        } else if ( dx < 0 && dy < 0)
                            id = nc/2 + id
                        else if ( dy < 0 && dx >= 0)
                            id = 3*nc / 4 + (nc/4 + id)
                        id = id >= (nc-1) ? 0 : id
                        return colorPicker.color(diskselection.currentIndex, circle, id)
                    }

                    Canvas {
                        id : diskcanvas
                        anchors.fill: parent
                        onPaint: {
                            var ctx = getContext("2d");
                            var radius = disk.diskRadius

                            var outerCircleSize = (colorPicker.numberOfCircles - 1) * 6

                            ctx.reset();

                            var cx = disk.width / 2;
                            var cy = disk.height / 2 ;
                            var gap = 0//2.0 * Math.PI / 137.8

                            var cellsize = radius / colorPicker.numberOfCircles
                            var numberOfCells = outerCircleSize
                            radius -= cellsize / 2
                            disk.radii = []
                            if ( radius > 0){
                                for(var circle = colorPicker.numberOfCircles; circle > 0; --circle ){
                                    for(var i = 0; i < numberOfCells; ++i){
                                        var clr = colorPicker.color(diskselection.currentIndex, circle-1,i)
                                        var radians = 2.0 * Math.PI * i / numberOfCells - 2.0 * Math.PI / (numberOfCells * 2)
                                        colorPicker.drawSegment(ctx,cx, cy,cellsize, radius, radians + gap/2, radians +   2.0 * Math.PI / numberOfCells - gap/2, clr)
                                    }
                                    ctx.beginPath();
                                    ctx.lineWidth = 1
                                    ctx.strokeStyle =  "grey"
                                    ctx.arc(cx, cy, radius + cellsize/2, 0, 2 * Math.PI);
                                    ctx.stroke()

                                    radius =  radius - cellsize
                                    numberOfCells -= 6
                                }
                            }

                            ctx.beginPath();
                            ctx.lineWidth = 1
                            ctx.strokeStyle =  "grey"
                            var clrCenter = colorPicker.color(diskselection.currentIndex,0,0)
                            ctx.fillStyle = clrCenter
                            ctx.fillRect(cx - cellsize/2 , cy-cellsize / 2, cellsize,cellsize);
                            ctx.stroke()
                        }
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                colorPicker.lastSelectedColor = disk.getColor(mouseX, mouseY,true)
                                comboBox.state = ""
								if ( closeCallBack)
									closeCallBack(colorPicker.lastSelectedColor)
                            }
                            onPositionChanged: {
                                var clr = disk.getColor(mouseX, mouseY,false)
                                colorInfo.text = clr.toString()
                            }
                        }
                    }


      
                }
            }
        }
    }
    states: State {
        name: "dropDown";
        PropertyChanges { target: dropDown; height:dropdownheight }
    }

    transitions: Transition {
        NumberAnimation { target: dropDown; properties: "height"; easing.type: Easing.OutExpo; duration: 400 }
    }
}
