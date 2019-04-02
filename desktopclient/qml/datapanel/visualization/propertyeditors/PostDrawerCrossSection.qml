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
			ctx.fillStyle = "white"
            ctx.ellipse(p.x - 5, p.y-5, 10,10)
			ctx.fill()
			ctx.lineWidth = 1;
			ctx.strokeStyle = "black"
            ctx.ellipse(p.x - 5, p.y-5, 10,10)
			crossSectionPostdrawer.drawStrokedText(ctx, p.label, p.x + 7, p.y + 7)
        }
        ctx.stroke();
    }

	function drawStrokedText(ctx, text, x, y) {
		ctx.font = '12px sans-serif';
		ctx.strokeStyle = 'black';
		ctx.lineWidth = 2;
		ctx.strokeText(text, x, y);
		ctx.fillStyle = 'white';
		ctx.fillText(text, x, y);
	}
}