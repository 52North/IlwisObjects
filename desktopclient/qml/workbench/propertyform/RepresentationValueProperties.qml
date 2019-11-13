import QtQuick 2.2
import QtQuick.Controls 1.1 as QC1
import QtQuick.Controls 2.3 as QC2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Item {
    id : dropItem
    anchors.fill : parent

    function storeData() {
        if ( propertyForm.editable){
			var result = {}
			var def = ""
			for(var i=0; i < ranges.count; ++i){
				if ( def != "")
				def += ";"
				var obj = ranges.get(i)
				def += obj.nmin + ":" + obj.nmax + "|" + obj.cmin + "|" + obj.cmax
			}
			result["definition"] = def
			setProperty("representationvalue", result)
			store()
        }
    }
	
	ListModel {
	id : ranges

	}
	
    clip:true

    Rectangle {
        id : inner
        height: 580
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5

		Column {
			id : tablepart
			width : parent.width
			height : parent.height - 20
			spacing : 4
			z: 100
			Row {
				spacing : 4
				QC1.CheckBox {
					id : rel
					width : 120
					height : 22
					checked : getProperty("relative") == "true"
					text : qsTr("Relative ranges")
					onCheckedChanged : {
						ranges.clear()
					}
					enabled : propertyForm.editable
				}
				Controls.TextEditLabelPair{
						id : steps
						width : 200
						height : 20
						labelWidth : 70
						labelText : qsTr("Steps")
						regexvalidator: /^\+?(0|[1-9]\d*)$/
						content : getProperty("steps")
						readOnly : !propertyForm.editable
				}
			}

			Column {
				id : rest
				width : parent.width
				height : 280
				spacing : 4
				QC1.TableView {
					id : itemtable
					width : parent.width
					height : 120
					property int steps : itemtable.getSteps(steps.content)

					function getSteps(val){
						var n = parseInt(val)
						if ( n <= 0)
							n = 20
						return n
					}

					function getColor(current, idx) {
						var obj = ranges.get(current)
						if ( !obj)
							return ""  
						var cmin = obj.cmin
						var cmax = obj.cmax
						var t1, t2, rgbMin, rgbMax
						if ( cmin.length > 7){
							t1 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmin);
							t2 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmax);
							rgbMin = { r: parseInt(t1[2], 16),  g: parseInt(t1[3], 16),  b: parseInt(t1[4], 16) }
							rgbMax = { r: parseInt(t2[2], 16),  g: parseInt(t2[3], 16),  b: parseInt(t2[4], 16) }
						}else {
							t1 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmin);
							t2 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmax);
							rgbMin = { r: parseInt(t1[1], 16),  g: parseInt(t1[2], 16),  b: parseInt(t1[3], 16) }
							rgbMax = { r: parseInt(t2[1], 16),  g: parseInt(t2[2], 16),  b: parseInt(t2[3], 16) }
						}
						var st = itemtable.steps > 1 ? itemtable.steps - 1 : 1
						var delta = (rgbMax.r - rgbMin.r)/ st;

						var red = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.r + delta * idx)));
						delta = (rgbMax.g - rgbMin.g) /st
						var green = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.g + delta * idx)));
						delta = (rgbMax.b - rgbMin.b)/ st
						var blue = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.b + delta * idx)));
						var res = "#" + ((1 << 24) + (red << 16) + (green << 8) + blue).toString(16).slice(1);
						return res

					}

					QC1.TableViewColumn{
						title : qsTr("min");
						role : "nmin"
						width : 50
						delegate: 	Text {
								text: styleData.value
								verticalAlignment:Text.AlignVCenter
								font.pixelSize: 10
								elide: Text.ElideMiddle
								x : 4
							}
					}
					QC1.TableViewColumn{
						title : qsTr("max");
						role : "nmax"
						width : 50
						delegate: 	Text {
								text: styleData.value
								verticalAlignment:Text.AlignVCenter
								font.pixelSize: 10
								elide: Text.ElideMiddle
								x : 4
							}
					}
					QC1.TableViewColumn{
						id : prescol
						title : qsTr("Presentation");
						width : parent.width - 160
						delegate:	Row {
							Repeater {
								id : local
								model : itemtable.steps
								width : 150
								Rectangle {
									width : prescol.width / itemtable.steps
									height : 20
									color : itemtable.getColor(styleData.row, index)
								}
							}
						}
					}
					rowDelegate: Rectangle {
						id : rowdelegate
						height :20
						color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")

						MouseArea {
							width : parent.width
							height : 25
							onClicked : {
								if ( ranges.count > 0 && propertyForm.editable){
									cranges.visible = true
									cranges.setValues(styleData.row)
								}
							}
						}
				
					}
					Component.onCompleted : {
						var definition = getProperty("definition")
						if ( definition != "?"){
							var elements = definition.split(";")
							for(var i = 0; i < elements.length; ++i){
								var element = elements[i]
								var groupdef = element.split("|")
								var limits = groupdef[0].split(":")
								var obj1 = {nmin : limits[0], nmax :limits[1], cmin : groupdef[1], cmax : groupdef[2] }
								ranges.append(obj1) 
							}
							model = ranges
						}
					}
				}

				Item {
					width : parent.width
					height : 22
					visible : propertyForm.editable
					Controls.PushButton{
						id : newbut
						width : 100
						height : 22
						text : qsTr("New Range")
						anchors.right : parent.right
						anchors.rightMargin : 10
						visible : propertyForm.editable  
						onClicked : {
							cranges.visible = true
							if ( ranges.count == 0){
								if ( rel.checked){
									nummin.content = 0
									nummax.content = 1
								}
							} else if ( cranges.editIndex == -1 && ranges.count >= 1){
								colormin.initialColor = ranges.get(ranges.count-1).cmax
								colormax.initialColor = "#000000"
							}else if ( cranges.editIndex >=0){
								colormax.initialColor = "#000000"
								colormin.initialColor = ranges.get(cranges.editIndex).cmax
								if ( cranges.editIndex < ranges.count - 1) // in between
									colormax.initialColor = ranges.get(cranges.editIndex + 1).cmin
							}
						}
					}
					Controls.PushButton{
						id : addbut
						width : 100
						height : 22
						text : "Delete"
						anchors.right :newbut.left
						anchors.leftMargin : 10
						visible : cranges.editIndex != -1 && propertyForm.editable

						onClicked : {
							cranges.visible = false
							ranges.remove(cranges.editIndex)
							cranges.editIndex = -1
						}
					}
				}

				Column {
					id : cranges
					width : parent.width - 20
					spacing : 4
					height : 80
					visible : false
					z : 100
					property var editIndex : -1 && propertyForm.editable

					function setValues(idx) {
						nummin.content = ranges.get(idx).nmin
						nummax.content = ranges.get(idx).nmax
						colormin.initialColor = ranges.get(idx).cmin
						colormax.initialColor = ranges.get(idx).cmax
						cranges.editIndex = idx;
					}
					Row {
						spacing : 4
						z : 12
						Controls.TextEditLabelPair{
							id : nummin
							width : 150
							height : 20
							labelWidth : 70
							regexvalidator : rel.checked ? /^(0+\.?|0*\.\d+|0*1(\.0*)?)$/ : /[\d|,|.|e|E|\+]+/
							labelText : qsTr("Start")
						}
						Controls.ColorPicker2{
							id : colormin
							width : 200
							height : 20
							initialColor : "#ffffff" 
						}
					}
					Row {
						spacing : 4
						z : 10
						Controls.TextEditLabelPair{
							id : nummax
							width : 150
							height : 20
							labelWidth : 70
							regexvalidator : rel.checked ? /^(0+\.?|0*\.\d+|0*1(\.0*)?)$/ : /[\d|,|.|e|E|\+]+/
							labelText : qsTr("End")
						}
						Controls.ColorPicker2{
							id : colormax
							width : 200
							height : 20

							initialColor : "#000000" 
						}
					}
					Controls.PushButton{
						width : 100
						height : 22
						text : cranges.editIndex == -1 ? "Add" : "Update"
						anchors.right : parent.right
						anchors.rightMargin : 10
						onClicked : {

							var ntmin = parseFloat(nummin.content)
							var ntmax = parseFloat(nummax.content)
							if ( ntmin >= ntmax || isNaN(ntmin) || isNaN(ntmax))
								return
							var rng = getProperty("numericrange")
							if ( rng){
								var parts = rng.split("|")
								if ( parts.length == 3){
									var m1 = parseFloat(parts[0])
									var m2 = parseFloat(parts[1])
									if ( ntmin < m1 || ntmax >m2)
										return
								}
							}
							if ( cranges.editIndex != -1)
								ranges.remove(cranges.editIndex)
							cranges.editIndex = -1
							var index = -1
							for(var i=0; i < ranges.count; ++i){
								var item = ranges.get(i)
								if ( ntmin <= item.nmax && ntmin >=  item.nmin)
									return
								if ( ntmax >= item.nmin && ntmax <=  item.nmax)
									return
								if ( ntmin < item.nmin && ntmax < item.nmax  ){
									index = i
									break;
								}
							}

							cranges.visible = false
							if ( cranges.editIndex == -1) {
								var obj1 = {nmin : ntmin.toString(), nmax : ntmax.toString(), cmin : colormin.selectedColor.toString(), cmax : colormax.selectedColor.toString() }
								if ( index == -1)
									ranges.append(obj1)
								else
									ranges.insert(index, obj1)
							}
							//itemtable.model = null
							//itemtable.model = ranges
							bar.items = makeLabels()
							bar.requestPaint()
						}
					}
				}
				Controls.LegendBar {
					id : bar
					width : Math.min(350, parent.width)
					x : 10
					height : 40
					items : makeLabels()
					colorFunc : colorf
					visible : ranges.count > 0
				}
			}
		}

	}

	function makeLabels() {
		var out = []
		if ( ranges.count > 0) {
			var tmin = ranges.get(0).nmin
			var tmax = ranges.get(ranges.count - 1).nmax

			var list = uicontext.roundedList(tmin, tmax, 0) 

			for(var i=0; i < list.length; ++i){
				out.push({label : list[i]})	
			}
		}
		return out
	}

	function colorf(dummy, frac){
		if ( ranges.count == 0)
			return "" 
		var tmin = ranges.get(0).nmin;
		var tmax = ranges.get(ranges.count - 1).nmax
		var delta = tmax - tmin
		for(var i=0; i<ranges.count; ++i){
			var f1 = (ranges.get(i).nmin - tmin) / delta
			var f2 = (ranges.get(i).nmax - tmin) / delta
			if ( frac >= f1 && frac <= f2){
				var delta2 = f2 - f1
				var cmin = ranges.get(i).cmin
				var cmax = ranges.get(i).cmax
				var t1, t2, rgbMin, rgbMax
				if ( cmin.length > 7){
					t1 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmin);
					t2 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmax);
					rgbMin = { r: parseInt(t1[2], 16),  g: parseInt(t1[3], 16),  b: parseInt(t1[4], 16) }
					rgbMax = { r: parseInt(t2[2], 16),  g: parseInt(t2[3], 16),  b: parseInt(t2[4], 16) }
				}else {
					t1 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmin);
					t2 = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(cmax);
					rgbMin = { r: parseInt(t1[1], 16),  g: parseInt(t1[2], 16),  b: parseInt(t1[3], 16) }
					rgbMax = { r: parseInt(t2[1], 16),  g: parseInt(t2[2], 16),  b: parseInt(t2[3], 16) }
				}
				var mult = ((frac - f1)/delta2)
				var dfr = mult * (rgbMax.r - rgbMin.r)
				var red = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.r + dfr)));
				var dfg = mult * (rgbMax.g - rgbMin.g)
				var green = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.g + dfg)));
				var dfb = mult * (rgbMax.b - rgbMin.b)
				var blue = Math.round(Math.max(0.0,Math.min(255.0,rgbMin.b + dfb)));
				var res = "#" + ((1 << 24) + (red << 16) + (green << 8) + blue).toString(16).slice(1);

				return res;
			}
		}
		return "" 
	}
}
