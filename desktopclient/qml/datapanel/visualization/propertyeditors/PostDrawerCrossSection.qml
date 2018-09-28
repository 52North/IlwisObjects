import LayerManager 1.0
import UIContextModel 1.0
import QtQuick 2.0

Item {
    id : crossSectionPostdrawer
    anchors.fill : parent

    function draw(ctx, editor){
        antialiasing = false
        ctx.strokeStyle = "#ff4500"
        ctx.lineWidth = 1;
        ctx.font = '12px sans-serif'
        var points = editor.pinLocation4screen
        for(var i=0; i < points.length; ++i){
            var p = points[i]
            ctx.ellipse(p.x - 5, p.y-5, 10,10)
            ctx.text(p.label, p.x + 7, p.y + 7)
        }
        ctx.stroke();
    }
}