import QtQuick 2.1
import RepresentationElement 1.0
import VisualAttribute 1.0
import LayerModel 1.0
import "../Global.js" as Global

Item {
	property var items
	property var colorFunc

	function requestPaint() {
		rprCanvas.requestPaint()
	}

	Canvas{
		id : rprCanvas
		anchors.fill : parent
		property bool isDirty : true
		renderTarget:  Canvas.FramebufferObject
		Rectangle{
			x : 0
			y : 19
			width: parent.width - 14
			height : 22
			color : "transparent"
			border.color: "black"
		}

		onPaint : {
			var ctx = getContext("2d")
			clear(ctx)
			//antialiasing = false
			drawArea(ctx, width - 15, height)
			drawBars(ctx, width - 15, height)

		}
	}

	function drawArea(ctx, w, h ){
		ctx.save();
		for(var j =0; j < w; j++){
			ctx.beginPath()
			var frac = j / w
			var color = colorFunc("", frac)
			ctx.beginPath()
			ctx.lineWidth = 1
			ctx.strokeStyle = color
			ctx.moveTo(j ,20)
			ctx.lineTo(j, 40)
			ctx.stroke()

			}
		ctx.restore()
	}

	function setText(ctx, label, x, below){
		//ctx.lineWidth = 0.4
		var textW = ctx.measureText(label)
		ctx.font = "8pt sans-serif";
		ctx.fillText(label, Math.max(0,x - textW.width / 2), below ? 55 : 14)
		ctx.stroke()
	}

	function drawBars(ctx, w, h ){ 
		ctx.save()
		var color = Qt.rgba(50,50,50,1)
		ctx.strokeStyle = color
		if ( items == null ||  items.length === 0){
			return
		}
		var step = Math.floor(w * item)
		var x = 0
		var below = true
		var nstart = Number(items[0].label)
		var nlast = Number(items[items.length - 1].label)
		var nprev =  Number(items[items.length - 2].label)
		var f =  w / (nlast - nstart)
		ctx.beginPath()
		for(var j =0; j < items.length - 1; ++j){
			var current = Number(items[j].label)
			var  p = (current - nstart) * f
			ctx.moveTo(p,20)
			ctx.lineTo(p,45)
			ctx.stroke()
			setText(ctx,items[j].label, p, below)
			below = false
		}
		ctx.beginPath()
		ctx.moveTo(w,20)
		ctx.lineTo(w,45)
		ctx.stroke()
		setText(ctx, items[items.length - 1].label,w, true)

		ctx.restore()
	}

	function clear(ctx) {
		ctx.reset();
		ctx.clearRect(0, 0, width, height);
		ctx.stroke();
	}
}