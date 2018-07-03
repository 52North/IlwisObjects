import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Item {
    id : slider
    height : 50
    width : parent.width
    property var model
    property var selectedMarker : -1
    property var minValue : -1
    property var maxValue : -1
    property var resolution : 0
    signal markerPositions(var positions)

    Text {
        width : 20
        anchors.bottom : bar.top
        anchors.left : bar.left
        text : minValue.toFixed(resolution)
        font.pointSize : 8
    }
    Text {
        width : 20
        anchors.bottom : bar.top
        anchors.right : bar.right
        text : maxValue.toFixed(resolution)
        font.pointSize : 8
    }
    Rectangle {
        y: 15
        height : slider.height - 37
        width : parent.width
        border.width : 1
        id : bar

        MouseArea{
            width: parent.width
            height:  parent.height
            onPressed: {
                for(var i=0; i < model.length; ++i){
                    var item = model[i];
                    var pos = item.position * bar.width
                        
                    if ( pos >= mouseX - 2 && pos <= mouseX + 2){
                        selectedMarker = i;
                        break;
                    }
                }
            }
            onPositionChanged : {
                if ( selectedMarker != -1){
                    var pos = mouseX / bar.width
                    var posNext = selectedMarker < model.length - 1 ? model[selectedMarker + 1].position * bar.width : bar.width
                    var posPrevious = selectedMarker > 0 ? model[selectedMarker -1].position * bar.width : 0
                    if ( mouseX >= posPrevious && mouseX <= posNext) 
                        model[selectedMarker].position = pos;
                    var positions = []
                    for(var i=0; i < model.length; ++i){
                        var v = (maxValue - minValue) * model[i].position
                        positions.push(v)
                    }
                    markerPositions(positions)
                    
                    canvas.requestPaint()
                 }
            }
            onReleased : {
                selectedMarker = -1
                canvas.requestPaint()
            }
        }
    }
    Canvas {
        id : canvas
        width : bar.width
        height : bar.height + 30
        y : 15
        z : 10
        onPaint: {
            if (model){
                var ctx = getContext("2d");
                ctx.reset();
                ctx.clearRect(0,0,width, height);
                ctx.beginPath()
                ctx.lineWidth = 1
                ctx.strokeStyle = 'rgb(0,0, 0)' 
                ctx.font = '10px sans-serif'
                ctx.stroke()
                for(var i = 0 ; i < model.length; ++i){
                    ctx.beginPath();
                        if ( i == selectedMarker)
                        ctx.strokeStyle = 'rgb(0, 255, 0)'
                    else
                        ctx.strokeStyle = 'rgb(128,128, 128)' 
                    var baseX = model[i].position * bar.width
                    var baseY = bar.height
                    ctx.moveTo(baseX, 0)
                    ctx.lineTo(baseX, baseY)
                    ctx.stroke()
                    ctx.beginPath()
                    if ( i == selectedMarker)
                        ctx.fillStyle = 'rgb(0, 255, 0)'
                    else
                        ctx.fillStyle = 'rgb(128,128, 128)'

                    ctx.lineTo(baseX - 5, baseY + 5)
                    ctx.lineTo(baseX + 5, baseY + 5)
                    ctx.lineTo(baseX, baseY)
                    ctx.fill()
                    ctx.beginPath()
                    ctx.lineWidth = 1
                     ctx.strokeStyle = 'rgb(0,0, 0)' 
                    var v = (maxValue - minValue) * model[i].position
                    v = v.toFixed(resolution)
                    var m = ctx.measureText(v)
                    ctx.text(v ,baseX - m.width  /2, baseY + 15)
                    ctx.stroke()
                }
            }
        }
    }

    function paint(){
        canvas.requestPaint()
    }

}