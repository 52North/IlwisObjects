import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import "../Global.js" as Global
import "../controls" as Controls

Item {
	property var slices : []
	property int numberOfSteps : 10
	property real minimumValue: 0
	property variant maximumValue : 10
	property variant overflowOffset : 0
	property variant overflowCount : 0
	property bool topValueMax : false
	property variant value : topValueMax?maximumValue:minimumValue
	property variant pressed : handle.pressed
	property var markerImage
	property var ringColor : Global.middlegreen
	property var backgroundColor :  "white"
	property var ringWidth : 60
	property alias currentValue : val.text
	width : height
	Rectangle {
		id  :ringRect
		width: height
		height: parent.height
		color : ringColor
		radius : height / 2
		Rectangle {
			width: height
			height: parent.height - ringWidth
			color : backgroundColor
			anchors.centerIn : parent
			radius : height / 2
		}

		Image {
			id: rect
			width: parent.width*0.17; height: parent.height*0.17
			source: markerImage //"../images/circularSliderHandle.png"
			x: -(Math.sin(value/(maximumValue-minimumValue)*-2*Math.PI)*(parent.width-width)/2)+parent.width/2-width/2
			y: -(Math.cos(value/(maximumValue-minimumValue)*-2*Math.PI)*(parent.height-height)/2)+parent.height/2-height/2
			fillMode: Image.PreserveAspectFit
			Text {
				id: val
				anchors.centerIn: parent;
				text: value
				font.pixelSize: 11
				color : "white"
				font.bold:true
			}
		}

		MouseArea{
			id: handle
			property int center: parent.width/2
			property int clickpointX
			property int clickpointY
			property bool isIn
			property int offset : 0
			anchors.fill: parent
			onPressed: {
				if (mouseX>rect.x&&mouseX<rect.x+rect.width&&mouseY>rect.y&&mouseY<rect.y+rect.height){
					isIn = true;
					clickpointX =  mouseX - rect.x -rect.width/2
					clickpointY =  mouseY - rect.y - rect.height/2
				} else {
					clickpointX = 0
					clickpointY = 0
					isIn = false
				}
				mouse.accepted = contains(mouseX, mouseY);
			}
			onMouseXChanged: {
				if (mouse.accepted){

					var res = nearestPointOnCircle(mouseX - center - clickpointX, mouseY - center - clickpointY, center, center, center-rect.width/2)
					if (numberOfSteps>0){
						var nearest = findNearestSlice(mouseX,mouseY,slices)
						res.x = slices[nearest].x;
						res.y = slices[nearest].y; 
					}
					var angle = Math.atan2(res.y -center,res.x -center) + Math.PI
					var cosA = Math.cos(angle)
					var sinA = Math.sin(angle)
					var currentAngle = (Math.atan2(res.x -center, res.y -center)+Math.PI)/(2*Math.PI)
					rect.x = res.x - rect.width/2 + cosA * rect.height/2
					rect.y = res.y - rect.height/2 + sinA * rect.height/2
					var range = (maximumValue-minimumValue)
					
					if (range>1) var roundAdjust = 1; else
					var roundAdjust = Math.pow(10,Math.ceil(-Math.log(range)+2))
					if (topValueMax && currentAngle==1) value = maximumValue;
					else value = maximumValue - currentAngle* range;
					var newValue = Math.round(value * roundAdjust)/roundAdjust 
					
					if ( (val.text == 0 + offset) && newValue  == maximumValue - 1 && overflowOffset > 0){
						offset = offset == 0 ? 12 : 0
					}
					else if ( (val.text == offset + maximumValue - 1) && newValue == 0  && overflowOffset > 0){
						offset = offset == 0 ? 12 : 0
					}
					val.text = newValue + offset
				}
			}
			Component.onCompleted: {
				slices = getSlices(numberOfSteps,center);
			}
			function contains(x, y) {
				var d = (width / 2);
				var ds = width/2-rect.width
				var dx = (x - width / 2);
				var dy = (y - height / 2);
				var ok = (d * d > dx * dx + dy * dy)&&(ds * ds < dx * dx + dy * dy);
				return ok
			}


		}
	}
	function nearestPointOnCircle(vX, vY, centerX, centerY, radius){
		var magV = Math.sqrt(vX*vX + vY*vY);
		var aX = centerX + vX / magV * radius;
		var aY = centerY + vY / magV * radius;
		return {x: aX, y:aY}
	}
	function getSlices(numberOfSlices,R){
		var slices = new Array();
		for (var i=0;i<numberOfSlices;i++){
			slices.push({x:R-Math.sin(2*i*Math.PI/numberOfSlices)*R,y:R-Math.cos(2*i*Math.PI/numberOfSlices)*R})
		}
		return slices;
	}
	function findNearestSlice(x,y, slices){
		var minDist=1000000;
		var nearest=-1;
		for (var i=0;i<slices.length;i++){
			var dist = distance(x,y,slices[i].x,slices[i].y)
			if (dist<minDist) {
				nearest = i;
				minDist = dist;
			}
		}
		return nearest;
	}
	function distance(x,y,xx,yy){
		return Math.sqrt(Math.pow(x-xx,2)+Math.pow(y-yy,2))
	}
}