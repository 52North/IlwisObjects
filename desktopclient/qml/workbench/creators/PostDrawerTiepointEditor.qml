import LayerManager 1.0
import UIContextModel 1.0
import ControlPointModel 1.0
import QtQuick 2.0

Item {
    id : tiepointsPostdrawer
    anchors.fill : parent

    function draw(ctx, editor){
        antialiasing = false
       // ctx.strokeStyle = Qt.rgba(255,0, 0, 1);
       // ctx.fillStyle = Qt.rgba(0,255, 0, 1);
       // ctx.lineWidth = 1;
        ctx.font = '14px sans-serif'
        var points = editor.controlPoints
        for(var i=0; i < points.length; ++i){
            var p = points[i]
            ctx.beginPath();
            ctx.fillStyle = p.color
            ctx.fillRect(p.columnScreen - 3, p.rowScreen-3, 6,6)
            ctx.stroke();
            ctx.beginPath();
            ctx.strokeStyle = p.color
            ctx.text(p.label, p.columnScreen + 7, p.rowScreen + 7)
            ctx.stroke();
            errorMarkerX(p, ctx, editor)
            errorMarkerY(p, ctx, editor)
      
        }
    }

    function errorMarkerX(p, ctx, editor){
        var lencolumn = p.columnError * 10

        if ( lencolumn != -10000000 && p.active) {
            ctx.beginPath();
            ctx.strokeStyle = "white"
            if ( lencolumn != 0){
                if ( lencolumn > 0 ){
                    ctx.moveTo(p.columnScreen + 3, p.rowScreen)
                    ctx.lineTo(p.columnScreen  + lencolumn + 3, p.rowScreen)
                    ctx.moveTo(p.columnScreen + lencolumn + 3, p.rowScreen - 4)
                    ctx.lineTo(p.columnScreen + lencolumn + 3, p.rowScreen  + 4)
                } else {
                    ctx.moveTo(p.columnScreen - 3, p.rowScreen)
                    ctx.lineTo(p.columnScreen  + lencolumn - 3, p.rowScreen)
                    ctx.moveTo(p.columnScreen + lencolumn - 3, p.rowScreen - 4)
                    ctx.lineTo(p.columnScreen + lencolumn - 3, p.rowScreen  + 4)

                }
            }
            ctx.stroke()
        }
    }

    function errorMarkerY(p, ctx, editor){
        var len = p.rowError * 10

        if ( len != 0 && len != -10000000 && p.active) {
            ctx.beginPath();
            ctx.strokeStyle = "white"
            if ( len > 0 ){
                ctx.moveTo(p.columnScreen , p.rowScreen + 3)
                ctx.lineTo(p.columnScreen , p.rowScreen + len + 3)
                ctx.moveTo(p.columnScreen - 4, p.rowScreen  + len + 3)
                ctx.lineTo(p.columnScreen   + 4, p.rowScreen + len + 3)
            } else {
                ctx.moveTo(p.columnScreen, p.rowScreen - 3)
                ctx.lineTo(p.columnScreen , p.rowScreen  + len - 3)
                ctx.moveTo(p.columnScreen -4, p.rowScreen + len - 3)
                ctx.lineTo(p.columnScreen +4, p.rowScreen + len - 3)

            } 
            ctx.stroke();
        }
    }
}