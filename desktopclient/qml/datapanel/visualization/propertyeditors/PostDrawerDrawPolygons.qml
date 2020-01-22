import LayerManager 1.0
import UIContextModel 1.0
import QtQuick 2.0

Item {
    id : crossSectionPostdrawer
    anchors.fill : parent

	function drawStrokedText(ctx, text, x, y) {
		ctx.font = '12px sans-serif';
		ctx.strokeStyle = 'blue';
		ctx.strokeText(text, x, y-5);
		ctx.fillStyle = 'white';
		ctx.fillText(text, x, y-5);
	}

    function draw(ctx, editor){
		if ( editor.useAOI) {
			for(var i=0; i < editor.polyCount; ++i){
				var pnts = editor.polygon(i);
				ctx.beginPath()
				ctx.lineWidth = 2;
				ctx.strokeStyle = "blue"
				for( var j=0; j < pnts.length; ++j){
					var pnt = pnts[j]
					if ( j == 0){
						ctx.moveTo(pnt.x, pnt.y)
						ctx.rect(pnt.x-2, pnt.y-2, 4, 4)
						ctx.moveTo(pnt.x, pnt.y)
					
						ctx.moveTo(pnt.x, pnt.y)
					}else {
						ctx.lineTo(pnt.x, pnt.y)
					}
				}
				if ( pnts.length > 2){
					pnt = pnts[0]
					ctx.lineTo(pnt.x, pnt.y)
				}
				ctx.stroke()

				ctx.beginPath()
				var pntStart = pnts[0]
				ctx.lineTo(pntStart.x + 6, pntStart.y -  6)
				drawStrokedText( ctx, i ,pntStart.x + 8, pntStart.y -  6) 
				ctx.stroke()
			}

		}
    }
}