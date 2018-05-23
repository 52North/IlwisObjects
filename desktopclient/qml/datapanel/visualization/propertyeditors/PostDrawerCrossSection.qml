import LayerManager 1.0
import UIContextModel 1.0
import QtQuick 2.0

Item {
    id : crossSectionPostdrawer
    anchors.fill : parent

    function draw(ctx, editor){
        ctx.strokeStyle = Qt.rgba(255, 0, 0, 1);
        ctx.lineWidth = 1;
        var points = editor.pinLocation4screen
        for(var i=0; i < points.length; ++i){
            var p = points[i]
            ctx.ellipse(p.x - 5, p.y-5, 10,10)
        }
        ctx.stroke();
    }
}