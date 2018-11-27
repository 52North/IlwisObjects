import QtQuick 2.1
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width : displayEditorColumn.width - 20
    height : 70
    y : 4
     property var editor



    Controls.TextEditLabelPair {
        function check(id){
            return editor.canUse(id)
        }
		labelWidth : 100
		labelText : qsTr("Representation")
        id : rprName
        width : parent.width - 20
        height : Global.rowHeight
        content : editor.representationName
        x : 10
        checkFunction: check

        onContentChanged: {
            editor.setRepresentation(content)
            rprCanvas.requestPaint()
        }
    }

    Canvas{
        id : rprCanvas
        anchors.top : rprName.bottom
        width : editorColumn1.width - 20
        property bool isDirty : true
        renderTarget:  Canvas.FramebufferObject
        x : 10
        height : 60
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
            var color = editor.color("", frac)
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
        var color = Qt.rgba(0,0,0,1)
        ctx.strokeStyle = color
        var items = editor.representationElements
        if ( items.length === 0)
            return

        var step = Math.floor(w * item)
        var x = 0
		var below = true
		var nstart = Number(items[0].label)
		var nlast = Number(items[items.length - 1].label)
		var nprev =  Number(items[items.length - 2].label)
		var f =  w / (nlast - nstart)
		console.debug("zzzzz2", nstart, nlast, f, w)
		ctx.beginPath()
        for(var j =0; j < items.length - 1; ++j){
			var current = Number(items[j].label)
			var  p = (current - nstart) * f
            ctx.moveTo(p,20)
            ctx.lineTo(p,45)
            ctx.stroke()
            setText(ctx,items[j].label, p, below)
			console.debug("zzzzzz",j, p, items[j].label,items[j].fraction)
			below = false
         }
        ctx.beginPath()
        ctx.moveTo(w,20)
        ctx.lineTo(w,45)
        ctx.stroke()
        setText(ctx, items[items.length - 1].label,w, true)
		console.debug("zzzzzz1",items.length - 1, w, items[items.length - 1].label)

        ctx.restore()
    }

    function clear(ctx) {
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        ctx.stroke();
    }
}
