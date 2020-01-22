.pragma library
var mainbackgroundcolor = "white";
var edgecolor = "lightgrey"
var alternatecolor1 = "lightgrey"
var alternatecolor2 = "white"
var alternatecolor3 = "#F7F9FC"
var alternatecolor4 = "#eee"
var alternatecolor5 = "#EAECEE"
var headerdark = "steelblue"
var headerlight = "lightsteelblue"
var formBackGround = "#F9F9F9"
var altFormBackGround = "#E6E6E6"
var selectedColor = "#99CCFF"
var selectedTabColor = "#B0C4DE"
var actionItemColor =  "#f2f0e9"
var alternateSelectedTabColor = "#87CEFA"
var notSelectedTabColor = "white"
var rowHeight = 20
var actionBarMaxHeight = 280
var actionBarMinHeight = 30
var panningAlong = 1;
var panningReverse = -1;
var darkestgreen = "#256E4D"
var darkgreen = "#2E8D63"
var middlegreen = "#6CBFA1"
var palegreen = "#B9CEBD"
var lightgreen = "#DDE6DE"
var lightestgreen = "#EBF0EC"
var lightergreen = "#E4EBE5"
var formBackGround = lightestgreen
var errorColor = "#ffcccc"

function isNumber(n) {
  return !isNaN(parseFloat(n)) && isFinite(n);
}

function calcZoomOutEnvelope(envelope, zoomposition, viewmanager, factor)
{
    if ( envelope !== ""){
        var x1 = envelope.minx;
        var x2 = envelope.maxx;
        var y1 = envelope.miny;
        var y2 = envelope.maxy;
        var w = Math.abs(x2 - x1);
        var h = Math.abs(y2 - y1);
        var nw = w * factor;
        var nh = h * factor;
        var cx = 0.5 * factor + zoomposition.x * (1.0 - factor);
        var cy = 0.5 * factor + zoomposition.y * (1.0 - factor);
        var nx1 = x1 + w * cx - nw / 2.0;
        var nx2 = x1 + w * cx + nw / 2.0;
        var ny1 = y1 + h * cy - nh / 2.0;
        var ny2 = y1 + h * cy + nh / 2.0;
        envelope = {minx: nx1, miny: ny1, maxx: nx2, maxy: ny2};
        if (typeof viewmanager.newZoomExtent == 'function') {
            viewmanager.newZoomExtent(nx1 + " " + ny1 + " " + nx2 + " " + ny2);
        }
    }
    return envelope;
}

function drawLine(canvas, ctx, pt1,pt2, selected, color, width){
    var fromx = pt1.x
    var fromy = pt1.y
    var tox = pt2.x
    var toy = pt2.y
    ctx.beginPath();
    ctx.moveTo(fromx, fromy);
    ctx.lineTo(tox, toy);
    if(selected)
    {
        ctx.strokeStyle = "red"
        ctx.lineWidth = width * 2
    }else {
        ctx.strokeStyle = color
        ctx.lineWidth = width * canvas.zoomScale
    }

    ctx.stroke()
}

function currentCatalogCorrectUrl(panel) {
    if ( !panel)
        return ""
    if ( panel.panelType === "catalog"){
        var url = panel.currentCatalog.url
        if ( url.indexOf("file://") !== 0) {
            return ""
        }
        return url
    }
    return ""
}

function drawArrow(canvas, ctx, pt1,pt2, selected){
    var fromx = pt1.x
    var fromy = pt1.y
    var tox = pt2.x
    var toy = pt2.y
    var headlen = 15 * canvas.zoomScale;   // length of head in pixels
    var angle = Math.atan2(toy-fromy,tox-fromx);

    ctx.beginPath();
    ctx.moveTo(fromx, fromy);
    ctx.lineTo(tox, toy);
    ctx.moveTo(tox, toy);
    ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/6),toy-headlen*Math.sin(angle-Math.PI/6));
    ctx.moveTo(tox, toy);
    ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/6),toy-headlen*Math.sin(angle+Math.PI/6));

    if(selected)
    {
        ctx.strokeStyle = "red"
        ctx.lineWidth = 5
    }else {
        ctx.strokeStyle = "blue"
        ctx.lineWidth = 3 * canvas.zoomScale
    }

    ctx.stroke()
}

function createfileUrlFromParts(left, right) {
    if (Qt.platform.os === "windows") {
        if (left.indexOf("file:/") == -1)
            return "file:///" + left + right;
        return left + right
    }
    if (left.indexOf("file:/") == -1)
        return "file://" + left + right;
    return left + right
}

function urlToLocalPath(path) {
    if (Qt.platform.os === "windows") {
        return path.replace("file:///","")
    }
    return path.replace("file://","")
}

function isInteger(valueType) {
    return valueType === "signedbyte" || valueType === "byte" || valueType === "int16" || valueType === "uint16" || valueType === "int32" || valueType === "uint32" || valueType ==="int64" || valueType === "uint64"
}

function isReal(valueType) {
    return valueType === "real32" || valueType === "real64"
}

function compareStrings (string1, string2, ignoreCase, useLocale) {
    if (ignoreCase) {
        if (useLocale) {
            string1 = string1.toLocaleLowerCase();
            string2 = string2.toLocaleLowerCase();
        }
        else {
            string1 = string1.toLowerCase();
            string2 = string2.toLowerCase();
        }
    }

    return string1 === string2;
}

function rainbow(numOfSteps, step) {
    // This function generates vibrant, "evenly spaced" colours (i.e. no clustering). This is ideal for creating easily distinguishable vibrant markers in Google Maps and other apps.
    // Adam Cole, 2011-Sept-14
    // HSV to RBG adapted from: http://mjijackson.com/2008/02/rgb-to-hsl-and-rgb-to-hsv-color-model-conversion-algorithms-in-javascript
    var r, g, b;
    var h = step / numOfSteps;
    var i = ~~(h * 6);
    var f = h * 6 - i;
    var q = 1 - f;
    switch (i % 6) {
        case 0: r = 1; g = f; b = 0; break;
        case 1: r = q; g = 1; b = 0; break;
        case 2: r = 0; g = 1; b = f; break;
        case 3: r = 0; g = q; b = 1; break;
        case 4: r = f; g = 0; b = 1; break;
        case 5: r = 1; g = 0; b = q; break;
    }
    var c = "#" + ("00" + (~ ~(r * 255)).toString(16)).slice(-2) + ("00" + (~ ~(g * 255)).toString(16)).slice(-2) + ("00" + (~ ~(b * 255)).toString(16)).slice(-2);
    return (c);
}