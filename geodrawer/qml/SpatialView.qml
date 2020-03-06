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
	property var postRenderCallBack

	function initSizes(){
		layermanager.rootLayer.initSizes(canvas.width, canvas.height,true)	
	}

	function reset() {
		var layerList = layermanager.topLevelLayers
		for(var i=layerList.length-1; i >=0; --i) {
			removeLayer(layerList[i])
		}
	}

	function removeLayer(layer){
		var layerList = layer.childLayers
        for ( var i=0; i < layerList.length; ++i){
            removeLayer(layerList[i])
        }
		if (layer.drawType == "raster")
			removeRasterLayer(layer)
		else if (layer.drawType != "?")
			removeFeatureLayer(layer)
	}

	function removeRasterLayer(layer){
		var sceneObject = scene.getObjectByName(layer.nodeId);
		for (var i = sceneObject.children.length - 1; i >= 0; i--) {
			var obj = sceneObject.children[i];
			if ( obj) {
				sceneObject.remove(obj);
				obj.material.tTexture.dispose();
				obj.material.dispose();
				obj.geometry.dispose();
			}
		}
		if (layer.tPalette != undefined) {
			layer.tPalette.dispose();
			layer.tPalette = undefined;
		}
        scene.remove(sceneObject);
    }

	function removeFeatureLayer(layer){
	    var sceneObject = scene.getObjectByName(layer.nodeId);
		// remove all previous renderings of this layer
        for (var i = sceneObject.children.length - 1; i >= 0; i--) {
            var obj = sceneObject.children[i];
            if (obj) {
                sceneObject.remove(obj);
                obj.material.dispose();
                obj.geometry.dispose();
            }
        }
        layer.clearMeshIndexes();
        scene.remove(sceneObject);
    }

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
				camera.left = layermanager.rootLayer.left;
				camera.right = layermanager.rootLayer.right;
				camera.top = layermanager.rootLayer.top;
				camera.bottom = layermanager.rootLayer.bottom;
				camera.updateProjectionMatrix();
				renderer.setSize( canvas.width, canvas.height );
				var enablePanAndZoomOut = layermanager.rootLayer.scrollInfo.xsizeperc < 1.0 || layermanager.rootLayer.scrollInfo.ysizeperc < 1.0;
				setZoomPanButton(enablePanAndZoomOut);
			}			
		}

        function setZoomPanButton (enablePanAndZoomOut) {
			 if (typeof layerview !== 'undefined'){
				if (layerview.activeLayerExtentsToolbar()){
					layerview.activeLayerExtentsToolbar().panButton.enabled = enablePanAndZoomOut
					layerview.activeLayerExtentsToolbar().zoomoutButton.enabled = enablePanAndZoomOut
				}
			}
        }

        function drawRasterAsColor(sceneObject,layer) {
            var removeQuads = layer.removeQuads;
            for (var i=0; i < removeQuads.length; ++i) {
                var obj = sceneObject.getObjectById(removeQuads[i])
                if ( obj){
                    sceneObject.remove(obj);
                    obj.material.tTexture.dispose();
                    obj.material.dispose();
                    obj.geometry.dispose();
                }
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
                        stretchscale_r: { type: "f", value: layer.stretch.scale_r },
                        stretchoffset_r: { type: "f", value: layer.stretch.offset_r },
                        stretchscale_g: { type: "f", value: layer.stretch.scale_g },
                        stretchoffset_g: { type: "f", value: layer.stretch.offset_g },
                        stretchscale_b: { type: "f", value: layer.stretch.scale_b },
                        stretchoffset_b: { type: "f", value: layer.stretch.offset_b },
						limitmax_r : { type: "f", value: layer.stretch.limitmax_r},
						limitmin_r : { type: "f", value: layer.stretch.limitmin_r},
						limitmax_g : { type: "f", value: layer.stretch.limitmax_g},
						limitmin_g : { type: "f", value: layer.stretch.limitmin_g},
						limitmax_b : { type: "f", value: layer.stretch.limitmax_b},
						limitmin_b : { type: "f", value: layer.stretch.limitmin_b},
						select_r : { type: "f", value: layer.stretch.select_r},
						select_g : { type: "f", value: layer.stretch.select_g},
						select_b : { type: "f", value: layer.stretch.select_b},
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
                if ( obj){
                    sceneObject.remove(obj);
                    obj.material.tTexture.dispose();
                    obj.material.dispose();
                    obj.geometry.dispose();
                }
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
				camera.left = layermanager.rootLayer.left;
				camera.right = layermanager.rootLayer.right;
				camera.top = layermanager.rootLayer.top;
				camera.bottom = layermanager.rootLayer.bottom;
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
					layer.renderReady = true
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
					layer.renderReady = true
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
					layer.renderReady = true
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
                    if ( mesh)
					    mesh.material.needsUpdate = true;
				}
			} else if ( propertyType == "layer"){
				var meshes = scene.getObjectByName(layer.nodeId);
                if ( meshes) {
				    if (layer.drawType != "raster") {
					    for(var i=0; i < meshes.children.length; ++i) {
						    var mesh = meshes.children[i];
                            if (mesh) {
                                mesh.visible = layer.vproperty("active");
                                mesh.material.opacity = layer.vproperty("opacity");
                            }
					    }
				    } else {
					    for(var i=0; i < meshes.children.length; ++i) {
						    var mesh = meshes.children[i];
                            if ( mesh){
						        mesh.visible = layer.vproperty("active");
						        mesh.material.uniforms.alpha.value = layer.vproperty("opacity");
                             }
					    }
				    }
                }
			} else if ( propertyType == "palette"){
				var palette = layer.palette;
                if ( layer.tPalette){
				    layer.tPalette.image.data.set(palette.data);
				    layer.tPalette.needsUpdate = true;
                }
			} else if ( propertyType == "colorcompstretch"){
			    var meshes = scene.getObjectByName(layer.nodeId);
			    if ( meshes) {
			        for(var i=0; i < meshes.children.length; ++i) {
			            var mesh = meshes.children[i];
			            if ( mesh){
							mesh.material.uniforms.stretchscale_r.value = layer.stretch.scale_r;
			                mesh.material.uniforms.stretchoffset_r.value = layer.stretch.offset_r;
			                mesh.material.uniforms.stretchscale_g.value = layer.stretch.scale_g;
			                mesh.material.uniforms.stretchoffset_g.value = layer.stretch.offset_g;
			                mesh.material.uniforms.stretchscale_b.value = layer.stretch.scale_b;
			                mesh.material.uniforms.stretchoffset_b.value = layer.stretch.offset_b;
							mesh.material.uniforms.limitmax_r.value = layer.stretch.limitmax_r;
							mesh.material.uniforms.limitmin_r.value = layer.stretch.limitmin_r;
							mesh.material.uniforms.limitmax_g.value = layer.stretch.limitmax_g;
							mesh.material.uniforms.limitmin_g.value = layer.stretch.limitmin_g;
							mesh.material.uniforms.limitmax_b.value = layer.stretch.limitmax_b;
							mesh.material.uniforms.limitmin_b.value = layer.stretch.limitmin_b;
							mesh.material.uniforms.select_r.value = layer.stretch.select_r;
							mesh.material.uniforms.select_g.value = layer.stretch.select_g;
							mesh.material.uniforms.select_b.value = layer.stretch.select_b;

			            }
			        }
			    }
			}
		}

        function setProperties(layer) {
            if ( layer && layer.isValid && !layer.isVirtual){
				var changedProperties = layer.changedProperties
                for(var j=0; j < changedProperties.length; ++j){
				    var property = changedProperties[j]
				    changeProperty(property, layer)
				    layer.removeFromChangedProperties(property) // note: the loop as it is here will not handle a list of properties in one go
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
				if(layermanager.removableLayer){
					var layer = layermanager.removableLayer
					if ( layer.drawType != "raster"){
						removeLayer(layer)
					}else {
						removeRasterLayer(layer)
					}
					layermanager.doneRemoving()
				}
				if ( layermanager.orderChanged){
					var newOrder = []
					var layerList = layermanager.topLevelLayers
					for(var i=layerList.length-1; i >=0; --i){
						var sceneObject = scene.getObjectByName(layerList[i].nodeId);
						if ( sceneObject){
							scene.remove(sceneObject)
							newOrder.push(sceneObject)
						}

					}
					for(var i=0; i < newOrder.length; ++i){
						scene.add(newOrder[i])
					}
					layermanager.orderChanged = false
				}
				var layerList = layermanager.topLevelLayers
				for(var i=layerList.length-1; i >=0; --i){
					setProperties(layerList[i])
				}
				renderer.render(scene, camera);

				if ( postRenderCallBack && layermanager.doPostRenderCallBack){
					postRenderCallBack()
					layermanager.doPostRenderCallBack = false
				}
                //console.log("current: geometries=" + renderer.info.memory.geometries)
                //console.log("current: textures=" + renderer.info.memory.textures)
			}
		}

		onInitializeGL: {
			initSizes()
			camera = new GL.THREE.OrthographicCamera( layermanager.rootLayer.left, layermanager.rootLayer.right, layermanager.rootLayer.top, layermanager.rootLayer.bottom, 0, 1 );
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
				'uniform sampler2D texture1;uniform float alpha;uniform float stretchscale_r;uniform float stretchoffset_r;' +
				'uniform float stretchscale_g;uniform float stretchoffset_g;uniform float stretchscale_b;uniform float stretchoffset_b;' +
				'uniform float limitmax_r;uniform float limitmin_r;uniform float limitmax_g;uniform float limitmin_g;uniform float limitmax_b;uniform float limitmin_b;' +
				'uniform float select_r; uniform float select_g; uniform float select_b;' +
				'bool adaptColor(bool useful, float clrComponent, float limitMin, float limitMax);' +
				'bool adaptColor(bool useful, float clrComponent, float limitMin, float limitMax) {' +
						'bool used  = true;' +
						'if ( useful){'+
							//'float delta = abs(clrComponent - limitMax);'+
							//'used = delta < 0.01;'+ 
							'used = clrComponent >= limitMin && clrComponent <= limitMax;'+
						'}'+ 
						'return(used);'+
				'}'+
				'void main() { ' +
					'gl_FragColor = texture2D(texture1,vUv);' +
					'bool useRed = limitmax_r != 9999999.0;'+ 
					'bool useGreen = limitmax_g != 9999999.0;'+ 
					'bool useBlue = limitmax_b != 9999999.0 ;'+ 
					'if ( useRed || useGreen || useBlue){'+
						'bool redUsed = adaptColor(useRed, gl_FragColor.r, limitmin_r, limitmax_r);'+ 
						'bool greenUsed = adaptColor(useGreen, gl_FragColor.g, limitmin_g,  limitmax_g);'+ 
						'bool blueUsed = adaptColor(useBlue, gl_FragColor.b, limitmin_b,  limitmax_b);'+ 		
						'if ( redUsed  && greenUsed && blueUsed){'+
							'gl_FragColor.r = select_r;'+ 
							'gl_FragColor.g = select_g;'+ 
							'gl_FragColor.b = select_b;'+ 
						'}'+
					'} else {'+ 
 					    'gl_FragColor.r *= stretchscale_r;' +
						'gl_FragColor.r += stretchoffset_r;' +
						'gl_FragColor.g *= stretchscale_g;' +
						'gl_FragColor.g += stretchoffset_g;' +
						'gl_FragColor.b *= stretchscale_b;' +
						'gl_FragColor.b += stretchoffset_b;' +
						'gl_FragColor.a *= alpha;'+ 
					'}' + 
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
