import QtQuick 2.7
import QtCanvas3D 1.0
import "three.js" as GL


Item {
	id : mapdrawer
	anchors.fill : parent
	anchors.margins :1

	property color backgroundColor: "#FFFFFF"
	property var camera
	property var scene
	property var renderer
	property int zorder : 0
	property bool drawing : false
	property int mi : -1
	property var colorShaderMaterialTemplate
	property var paletteShaderMaterialTemplate

 	Canvas3D {
        id : canvas

        anchors.fill: parent

        Component.onCompleted : {
        }

		onWidthChanged :{
			updateAfterSizeChange()
		}
		onHeightChanged :{
			updateAfterSizeChange();
		}

		function updateAfterSizeChange(){
			if ( camera && renderer){
				layermanager.rootLayer.initSizes(canvas.width, canvas.height, false);
				camera.left = -layermanager.rootLayer.width/2.0;
				camera.right = layermanager.rootLayer.width/2.0;
				camera.top = layermanager.rootLayer.height/2.0;
				camera.bottom = -layermanager.rootLayer.height/2.0;
				camera.updateProjectionMatrix();
				renderer.setSize( canvas.width, canvas.height );
				var enablePanAndZoomOut = layermanager.rootLayer.scrollInfo.xsizeperc < 1.0 || layermanager.rootLayer.scrollInfo.ysizeperc < 1.0;
				setZoomPanButton(enablePanAndZoomOut);
			}			
		}

        function setZoomPanButton (enablePanAndZoomOut) {
            layerview.maptools.panButton.enabled = enablePanAndZoomOut
            layerview.maptools.zoomoutButton.enabled = enablePanAndZoomOut
        }

        function drawRasterAsColor(sceneObject,layer) {
            var removeQuads = layer.removeQuads;
            for (var i=0; i < removeQuads.length; ++i) {
                var obj = sceneObject.getObjectById(removeQuads[i])
                sceneObject.remove(obj);
                obj.material.tTexture.dispose();
                obj.material.dispose();
                obj.geometry.dispose();
            }
            var quads = layer.numberOfBuffers("rastercoverage");
            for (var i=0; i < quads;++i){
                var texture = layer.texture(i);
                if (texture.valid) {
				    var tTexture = new GL.THREE.DataTexture(
					    new Uint8Array(texture.data),
					    texture.width,
					    texture.height,
					    GL.THREE.RGBAFormat,
					    GL.THREE.UnsignedByteType,
					    GL.THREE.UVMapping);
				    tTexture.needsUpdate = true;
				    var uniforms = {
                        alpha: { type: "f", value: layer.vproperty("opacity") },
                        texture1: { type: "t", value: tTexture },
                        uvst1: { type: "v4", value: new GL.THREE.Vector4(texture.uvmap.s, texture.uvmap.t, texture.uvmap.sscale, texture.uvmap.tscale)},
				    };
				    var material = colorShaderMaterialTemplate.clone();
				    material.uniforms = uniforms
				    material.tTexture = tTexture; // keep a copy
				    material.needsUpdate = true;
                    var geometry = new GL.THREE.BufferGeometry();
                    canvas.setGeometry(layer, i,"rastercoverage",geometry);
                    var uvs = new Float32Array(layer.uvs(i))
                    geometry.addAttribute('uv', new GL.THREE.BufferAttribute( uvs, 2 ) );
                    var quad = new GL.THREE.Mesh( geometry, material );
                    quad.name = layer.layerId;
                    quad.visible = layer.vproperty("active");
                    layer.setQuadId(i, quad.id);
                    quad.renderOrder = sceneObject.renderOrder;
                    sceneObject.add(quad);
                }
            }
		}

        function drawRasterAsValue(sceneObject,layer) {
            if (layer.tPalette == undefined) {
                var palette = layer.palette;
                layer.tPalette = new GL.THREE.DataTexture(
                    new Uint8Array(palette.data),
                    palette.width,
                    palette.height,
                    GL.THREE.RGBAFormat,
                    GL.THREE.UnsignedByteType);
                layer.tPalette.needsUpdate = true;
            }
            var removeQuads = layer.removeQuads;
            for (var i=0; i < removeQuads.length; ++i) {
                var obj = sceneObject.getObjectById(removeQuads[i])
                sceneObject.remove(obj);
                obj.material.tTexture.dispose();
                obj.material.dispose();
                obj.geometry.dispose();
            }
            var quads = layer.numberOfBuffers("rastercoverage");
            for (var i=0; i < quads;++i) {
                var texture = layer.texture(i);
                if (texture.valid) {
				    var tTexture = new GL.THREE.DataTexture(
					    new Uint8Array(texture.data),
					    texture.width,
					    texture.height,
					    GL.THREE.AlphaFormat,
					    GL.THREE.UnsignedByteType,
					    GL.THREE.UVMapping);
				    tTexture.needsUpdate = true;
				    var uniforms = {
					    alpha: { type: "f", value: layer.vproperty("opacity") },
					    texture1: { type: "t", value: tTexture },
					    uvst1: { type: "v4", value: new GL.THREE.Vector4(texture.uvmap.s, texture.uvmap.t, texture.uvmap.sscale, texture.uvmap.tscale)},
					    palette: { type: "t", value: layer.tPalette }
				    };
                    var material = paletteShaderMaterialTemplate.clone();
				    material.uniforms = uniforms
                    material.tTexture = tTexture; // keep a copy
				    material.needsUpdate = true;
                    var geometry = new GL.THREE.BufferGeometry();
                    canvas.setGeometry(layer, i,"rastercoverage",geometry);
                    var uvs = new Float32Array(layer.uvs(i))
                    geometry.addAttribute('uv', new GL.THREE.BufferAttribute( uvs, 2 ) );
                    var quad = new GL.THREE.Mesh( geometry, material );
                    quad.name = layer.layerId;
                    quad.visible = layer.vproperty("active");
                    layer.setQuadId(i, quad.id);
                    quad.renderOrder = sceneObject.renderOrder;
                    sceneObject.add(quad);
                }
            }
		}

		function setBackgroundColor(backgroundColor) {
			var str = ""+backgroundColor;
			var color = parseInt(str.substring(1), 16);
			if (renderer)
				renderer.setClearColor(color);
		}

		function updatePositions() {
			if ( camera){
				var cameraPosition = layermanager.rootLayer.cameraPosition;
				camera.position.set(cameraPosition.x,cameraPosition.y, 1);
				camera.left = -layermanager.rootLayer.width/2.0;
				camera.right = layermanager.rootLayer.width/2.0;
				camera.top = layermanager.rootLayer.height/2.0;
				camera.bottom = -layermanager.rootLayer.height/2.0;
				var pp = new GL.THREE.Vector3(cameraPosition.x,cameraPosition.y,0)
				camera.lookAt(pp);
				camera.updateProjectionMatrix();
			}
		}

	function setGeometry(layer, bufferIndex, type, geometry){
			var vertices = new Float32Array(layer.vertices(bufferIndex, type))

			geometry.addAttribute( 'position', new GL.THREE.BufferAttribute( vertices, 3 ) );
			if ( !layer.isSupportLayer && type != "rastercoverage"){
				var colors = new Float32Array(layer.colors(bufferIndex,type))
				geometry.addAttribute( 'color', new GL.THREE.BufferAttribute( colors, 3 ) );
			}
			if (type != "points") {
				var indices = new Uint16Array(layer.indices(bufferIndex,type))
				geometry.setIndex(new GL.THREE.BufferAttribute(indices, 1));
			}
		}

	function setScene(layer){
            var sceneObject = scene.getObjectByName(layer.nodeId);
            if (sceneObject == null) {
                sceneObject = new GL.THREE.Group();
                sceneObject.name = layer.nodeId;
                sceneObject.renderOrder = zorder++;
                scene.add(sceneObject);
            }
            if (layer.drawType != "raster") {
			    // remove all previous renderings of this layer
                for (i = sceneObject.children.length - 1; i >= 0; i--) {
                    sceneObject.remove(sceneObject.children[i]);
                }
			    layer.clearMeshIndexes()
			    //construct the meshes
		        for(var i=0; i < layer.numberOfBuffers("points");++i){
				    var geometry = new GL.THREE.BufferGeometry();
				    setGeometry(layer,i,"points",geometry);
				    var material = new GL.THREE.PointsMaterial( { size: 5, vertexColors: GL.THREE.VertexColors,sizeAttenuation : false, transparent : true,opacity : layer.vproperty("opacity") } );
				    var points = new GL.THREE.Points( geometry, material );
				    points.name = layer.nodeId;
				    points.visible = layer.vproperty("active");
				    layer.addMeshIndex(points.id);
				    points.renderOrder = sceneObject.renderOrder;
				    sceneObject.add( points );
			    }
		        for(var i=0; i < layer.numberOfBuffers("lines");++i){
				    var geometry = new GL.THREE.BufferGeometry();
				    canvas.setGeometry(layer, i,"lines",geometry);
				    var material;
				    if ( layer.isSupportLayer){
					    var clr = layer.vproperty("fixedlinecolor");
					    var linecolor = new GL.THREE.Color(clr.r, clr.g, clr.b);
					    material = new GL.THREE.LineBasicMaterial( {color: linecolor,linewidth: 1, transparent : true,opacity : layer.vproperty("opacity")});
				    }else {
					    material = new GL.THREE.LineBasicMaterial({  vertexColors: GL.THREE.VertexColors, transparent : true,opacity : layer.vproperty("opacity") });
				    }
				    var lines = new GL.THREE.LineSegments( geometry, material );
				    lines.name = layer.nodeId;
				    lines.visible = layer.vproperty("active");
				    layer.addMeshIndex(lines.id);
				    lines.renderOrder = sceneObject.renderOrder;
				    sceneObject.add( lines );
			    }
               // var n = Date.now()
		        for(var i=0; i < layer.numberOfBuffers("polygons");++i){
                   
				    var geometry = new GL.THREE.BufferGeometry();
				    canvas.setGeometry(layer, i,"polygons",geometry)
				    var material = new GL.THREE.MeshBasicMaterial({ side : GL.THREE.DoubleSide, vertexColors: GL.THREE.VertexColors, transparent : true,opacity : layer.vproperty("opacity") });
				    var polygons = new GL.THREE.Mesh( geometry, material );
				    polygons.name = layer.nodeId
				    polygons.visible = layer.vproperty("active")
				    layer.addMeshIndex(polygons.id)
				    polygons.renderOrder = sceneObject.renderOrder;
				    sceneObject.add( polygons );
			    }
                // var n2 = Date.now()
                // console.debug("duration=", n2, n, (n2 - n)/1000.0)
            } else { // "raster"
                if (layer.usesColorData) {
                    drawRasterAsColor(sceneObject,layer)
                } else {
                    drawRasterAsValue(sceneObject,layer)
                }
            }
			layer.updateGeometry = false
		}

		function getMesh(layer,id){
            var sceneObject = scene.getObjectByName(layer.nodeId);
            if (sceneObject != null) {
                var mesh = sceneObject.getObjectById(id);
                return mesh;
            }
			return null
		}

		function changeProperty(propertyType, layer){
			var ok = true
			if ( propertyType == "buffers"){
	            if ( layer.updateGeometry){
                    ok = layer.prepare(2) // ptGEOMETRY = 2
                }else{
					ok = layer.prepare(1) // ptRENDER = 1
				}
				if ( ok){
					setScene(layer)
				}
			} else if ( propertyType == "material") {
				for(var i=0; i < layer.meshCount(); ++i){
					var mesh =  getMesh(layer, layer.meshIndex(i))
					if ( mesh && (layer.drawType == "line")){
						var clr = layer.vproperty("fixedlinecolor")
						var linecolor = new GL.THREE.Color(clr.r, clr.g, clr.b)
						var linew = layer.vproperty("linewidth")
						var lineopacity = layer.vproperty("opacity")
						mesh.material = new GL.THREE.LineBasicMaterial( {color: linecolor, transparent : true,opacity : lineopacity})
					}
					mesh.material.needsUpdate = true;
				}
			} else if ( propertyType == "layer"){
				var meshes = scene.getObjectByName(layer.nodeId);
				if (layer.drawType != "raster") {
					for(var i=0; i < meshes.children.length; ++i) {
						var mesh = meshes.children[i];
						mesh.visible = layer.vproperty("active");
						mesh.material.opacity = layer.vproperty("opacity");
					}
				} else {
					for(var i=0; i < meshes.children.length; ++i) {
						var mesh = meshes.children[i];
						mesh.visible = layer.vproperty("active");
						mesh.material.uniforms.alpha.value = layer.vproperty("opacity");
					}
				}
			}
		}

        function setProperties(layer) {
            if ( layer && layer.isValid){
				var changedProperties = layer.changedProperties
                for(var j=0; j < changedProperties.length; ++j){
				    var property = changedProperties[j]
				    changeProperty(property, layer)
				    layer.removeFromChangedProperties(property)
				}
                var layerList = layer.childLayers
                for ( var i=0; i < layerList.length; ++i){
                    setProperties(layerList[i])
                }
            }
        }
        
		onPaintGL : {
			if (scene && camera){
				if ( layermanager.needUpdate){
					updatePositions()
					layermanager.needUpdate = false
				}
				var layerList = layermanager.topLevelLayers
				for(var i=layerList.length-1; i >=0; --i){
					setProperties(layerList[i])
				}
				renderer.render(scene, camera);
                //console.log("current: geometries=" + renderer.info.memory.geometries)
                //console.log("current: textures=" + renderer.info.memory.textures)
			}
		}

		onInitializeGL: {
			layermanager.rootLayer.initSizes(canvas.width, canvas.height,true)
			camera = new GL.THREE.OrthographicCamera( -layermanager.rootLayer.width/2.0, layermanager.rootLayer.width/2.0, layermanager.rootLayer.height/2.0, -layermanager.rootLayer.height/2.0, 0, 1 );
			var cameraPosition = layermanager.rootLayer.cameraPosition;
			camera.position.set(cameraPosition.x, cameraPosition.y, 1);
			camera.zoom = layermanager.rootLayer.zoomFactor
			scene = new GL.THREE.Scene();
			var pp = new GL.THREE.Vector3(cameraPosition.x, cameraPosition.y, 0);
			camera.lookAt(pp);
			camera.updateProjectionMatrix();

			renderer = new GL.THREE.Canvas3DRenderer(
				   { canvas: canvas, antialias: false, devicePixelRatio: canvas.devicePixelRatio });
			renderer.setPixelRatio(canvas.devicePixelRatio);
			renderer.setSize(canvas.width, canvas.height);
			canvas.setBackgroundColor(backgroundColor);

            colorShaderMaterialTemplate = new GL.THREE.ShaderMaterial({ side : GL.THREE.DoubleSide, transparent : true });
			colorShaderMaterialTemplate.vertexShader = 'varying vec2 vUv;uniform vec4 uvst1;void main() {gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);vUv=(uv-uvst1.st)*uvst1.pq;}'
			colorShaderMaterialTemplate.fragmentShader =
				'varying vec2 vUv;' +
				'uniform sampler2D texture1;uniform float alpha;' +
				'void main() { ' +
					'gl_FragColor = texture2D(texture1,vUv);' +
					'gl_FragColor.a *= alpha;' +
				'}'
            paletteShaderMaterialTemplate = new GL.THREE.ShaderMaterial({ side : GL.THREE.DoubleSide, transparent : true });
			paletteShaderMaterialTemplate.vertexShader = 'varying vec2 vUv;uniform vec4 uvst1;void main() {gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);vUv=(uv-uvst1.st)*uvst1.pq;}'
			paletteShaderMaterialTemplate.fragmentShader =
				'varying vec2 vUv;' +
				'uniform sampler2D texture1;uniform sampler2D palette;uniform float alpha;' +
				'void main() { ' +
					'float x = texture2D(texture1,vUv).a;' +
					'float y = 0.5;'+
					'gl_FragColor = texture2D(palette,vec2(x,y));' +
					'gl_FragColor.a *= alpha;' +
				'}'
		}
    }
}
