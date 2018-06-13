import LayerManager 1.0
import UIContextModel 1.0
import ControlPointModel 1.0
import QtQuick 2.0

Item {
    id : tiepointsPostdrawer
    anchors.fill : parent

    function draw(ctx, editor){
        antialiasing = false
        ctx.strokeStyle = Qt.rgba(255,0, 0, 1);
        ctx.lineWidth = 1;
        ctx.font = '12px sans-serif'
        var points = editor.controlPoints
        console.debug("drawing", points, points.length)
        for(var i=0; i < points.length; ++i){
            var p = points[i]
            console.debug("point ", p.column, p.row)
            ctx.ellipse(p.column - 5, p.row-5, 15,15)
        }
        ctx.stroke();
    }
}