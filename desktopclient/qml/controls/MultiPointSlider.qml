import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Canvas {
    id : slider
    height : 35
    width : parent.width
    property var model
    property var selectedMarker : -1
    property var minValue : -1
    property var maxValue : -1
    property var resolution : 0
    signal markerPositions(var positions)

    Rectangle {
        height : slider.height - 22
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
        height : slider.height
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
                var m = ctx.measureText(maxValue)
                ctx.text(minValue ,0 , 22)
                ctx.text(maxValue ,bar.width - m.width, 22)
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
                    ctx.text(v ,baseX - m.width  /2, baseY + 17)
                    ctx.stroke()
                }
            }
        }
    }

}