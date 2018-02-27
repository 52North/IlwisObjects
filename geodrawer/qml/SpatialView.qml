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
	property bool drawing : false
	property int mi : -1

 	Canvas3D {
        id : canvas

        anchors.fill: parent

        Component.onCompleted : {
            console.debug("canvas", width, height)
        }

		onWidthChanged :{
			updateAfterSizeChange()
		}
		onHeightChanged :{
			updateAfterSizeChange();
		}

		function updateAfterSizeChange(){
			if ( camera && renderer){
				layermanager.rootLayer.initSizes(canvas.width, canvas.height, false)
				var aspect = canvas.width / canvas.height; 
				var f = aspect >= 0 ? layermanager.rootLayer.height : layermanager.rootLayer.width
				camera.left = f * aspect/-2
				camera.right = f * aspect/2
				camera.top = f /2  
				camera.bottom =f /-2
				camera.aspect = aspect
				camera.updateProjectionMatrix();
				renderer.setSize( canvas.width, canvas.height );
			}			
		}
	

		function uvmapping(geometry) {
			geometry.computeBoundingBox();

			var max = geometry.boundingBox.max;
			var min = geometry.boundingBox.min;

			var offset = new THREE.Vector2(0 - min.x, 0 - min.y);
			var range = new THREE.Vector2(max.x - min.x, max.y - min.y);

			geometry.faceVertexUvs[0] = [];
			var faces = geometry.faces;

			for (var i = 0; i < geometry.faces.length; i++) {
				var v1 = geometry.vertices[faces[i].a];
				var v2 = geometry.vertices[faces[i].b];
				var v3 = geometry.vertices[faces[i].c];
		        geometry.faceVertexUvs[0].push([

				new THREE.Vector2((v1.x + offset.x) / range.x, (v1.y + offset.y) / range.y),
				new THREE.Vector2((v2.x + offset.x) / range.x, (v2.y + offset.y) / range.y),
				new THREE.Vector2((v3.x + offset.x) / range.x, (v3.y + offset.y) / range.y)
				]);
			}
			geometry.uvsNeedUpdate = true;
		}

		function drawRasterAsColor(){
		
		}

		function drawRasterAsValue(){
			var image = layer.image;
			for(var i=0; i < image.textureCount(); ++i){
				var texture = image.texture(i)
				var pixelData = texture.pixeldata
				var paletteData = texture.palette

				var gridData = texture.grid
				var h = texture.width
				var w = texture.height
				var arr = new Uint8Array(pixelData)
				var material = new THREE.ShaderMaterial({ transparent: true });
				material.vertexShader = 'varying vec2 vUv;void main() {gl_Position = vec4(position,1.0);vUv=uv;}'
				material.fragmentShader =
					'varying vec2 vUv;' +
					'uniform sampler2D texture1;uniform sampler2D palette;' +
					'void main() { ' +
						'float index = texture2D(texture1,vUv.xy).a * 255;' +
						'float x = floor(index-floor((index+0.5)/16)*16+0.5);'+
						'float y = int(index/16);'+
						'gl_FragColor = texture2D(palette,vec2(y/16,x/16));' +
					'}'

				var dataTexture = new THREE.DataTexture(
					arr,
					 w,
					 h,
					THREE.AlphaFormat,
					THREE.UnsignedByteType,
					THREE.UVMapping);
				dataTexture.needsUpdate = true

				var dpalette = new THREE.DataTexture(
					new Uint8Array(paletteData),
					16,
					16,
					THREE.RGBAFormat,
					THREE.UnsignedByteType);
				 dpalette.needsUpdate = true

				var uniforms = {
					texture1: { type: "t", value: dataTexture },
					palette: { type: "t", value: dpalette }
				};
    
				material.uniforms = uniforms
				material.needsUpdate = true

				var geom = new THREE.Geometry();

				for (var i = 0; i < gridData.length; i += 3) {
					geom.vertices.push(new THREE.Vector3(gridData[i], gridData[i + 1], gridData[i + 2]))
				}
				geom.faces.push(new THREE.Face3(0, 1, 2));
				geom.faces.push(new THREE.Face3(5, 4, 3));
				geom.computeFaceNormals();
				geom.computeVertexNormals();
				uvmapping(geom)


				mesh = new THREE.Mesh(geom, material);
				mesh.position.set(0, 0.0, 0.0);

				scene.add(mesh)
			}
		}

		function drawRaster(layer){
			if ( layer.usesColorData){
				drawRasterAsColor(layer)
			}else {
				drawRasterAsValue(layer)
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
				var aspect = canvas.width / canvas.height
				var frustumSize = aspect >= 0 ? layermanager.rootLayer.height : layermanager.rootLayer.width
				var cameraPosition = layermanager.rootLayer.cameraPosition
				camera.position.set(cameraPosition.x,cameraPosition.y, frustumSize);
				camera.zoom = layermanager.rootLayer.zoomFactor
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
		
			// remove all previous renderings of this layer

			for (var i = scene.children.length - 1; i >= 0; i--) {
				if(scene.children[i].name === layer.layerId){
						scene.remove(scene.children[i]);
					}
			}
			layer.clearMeshIndexes()
			//construct the meshes
		    for(var i=0; i < layer.numberOfBuffers("points");++i){
				var geometry = new GL.THREE.BufferGeometry();
				setGeometry(layer,i,"points",geometry)
				var material = new GL.THREE.PointsMaterial( { size: 5, vertexColors: GL.THREE.VertexColors,sizeAttenuation : false, transparent : true,opacity : layer.vproperty("opacity") } );
				var points = new GL.THREE.Points( geometry, material );
				points.name = layer.layerId
				points.visible = layer.vproperty("active")
                layer.addMeshIndex(points.id)
                scene.add( points );
			}
		    for(var i=0; i < layer.numberOfBuffers("lines");++i){
				var geometry = new GL.THREE.BufferGeometry();
				canvas.setGeometry(layer, i,"lines",geometry)
				var material
				if ( layer.isSupportLayer){
                	var clr = layer.vproperty("fixedlinecolor")
					var linecolor = new GL.THREE.Color(clr.r, clr.g, clr.b)
					material = new GL.THREE.LineBasicMaterial( {color: linecolor,linewidth: 1, transparent : true,opacity : layer.vproperty("opacity")})
				}else {
					material = new GL.THREE.LineBasicMaterial({  vertexColors: GL.THREE.VertexColors, transparent : true,opacity : layer.vproperty("opacity") });
				}
				var lines = new GL.THREE.LineSegments( geometry, material );
				lines.name = layer.layerId
				lines.visible = layer.vproperty("active")
				layer.addMeshIndex(lines.id)
				scene.add( lines );
			}
           // var n = Date.now()
		    for(var i=0; i < layer.numberOfBuffers("polygons");++i){
				var geometry = new GL.THREE.BufferGeometry();
				canvas.setGeometry(layer, i,"polygons",geometry)
				var material = new GL.THREE.MeshBasicMaterial({ side : GL.THREE.DoubleSide, vertexColors: GL.THREE.VertexColors, transparent : true,opacity : layer.vproperty("opacity") });
				var polygons = new GL.THREE.Mesh( geometry, material );
				polygons.name = layer.layerId
				polygons.visible = layer.vproperty("active")
				layer.addMeshIndex(polygons.id)
				scene.add( polygons );

			}
            // var n2 = Date.now()
            // console.debug("duration=", n2, n, (n2 - n)/1000.0)
            if (layer.drawType == "raster") {
			    var palette = layer.palette;
			    var tPalette = new GL.THREE.DataTexture(
				    new Uint8Array(palette.data),
				    palette.width,
				    palette.height,
				    GL.THREE.RGBAFormat,
				    GL.THREE.UnsignedByteType);
			    tPalette.needsUpdate = true
                var quads = layer.numberOfBuffers("rastercoverage");
                for (var i=0; i < quads;++i){
                    var geometry = new GL.THREE.BufferGeometry();
                    canvas.setGeometry(layer, i,"rastercoverage",geometry);
                    var uvs = new Float32Array(layer.uvs(i))
                    geometry.addAttribute('uv', new GL.THREE.BufferAttribute( uvs, 2 ) );
                    var material;
                    var texture = layer.texture(i);
                    if (texture.valid) {
				        var tTexture = new GL.THREE.DataTexture(
					        new Uint8Array(texture.data),
					        texture.width,
					        texture.height,
					        GL.THREE.AlphaFormat,
					        GL.THREE.UnsignedByteType,
					        GL.THREE.UVMapping);
				        tTexture.needsUpdate = true
				        var uniforms = {
					        texture1: { type: "t", value: tTexture },
                            uvst1: { type: "v4", value: new GL.THREE.Vector4(texture.uvmap.s, texture.uvmap.t, texture.uvmap.sscale, texture.uvmap.tscale)},
					        palette: { type: "t", value: tPalette }
				        };
                        material = new GL.THREE.ShaderMaterial({ side : GL.THREE.DoubleSide, transparent : true });
				        material.vertexShader = 'varying vec2 vUv;uniform vec4 uvst1;void main() {gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);vUv=(uv-uvst1.st)*uvst1.pq;}'
				        material.fragmentShader =
					        'varying vec2 vUv;' +
					        'uniform sampler2D texture1;uniform sampler2D palette;' +
					        'void main() { ' +
						        'float x = texture2D(texture1,vUv).a;' +
						        'float y = 0.5;'+
						        'gl_FragColor = texture2D(palette,vec2(x,y));' +
					        '}'
				        material.uniforms = uniforms
				        material.needsUpdate = true
                    } else
                        material = new GL.THREE.MeshBasicMaterial({ side : GL.THREE.DoubleSide, color: 0x00FF00, transparent : false, opacity : 1.0 });
                    var quad = new GL.THREE.Mesh( geometry, material );
                    quad.name = layer.layerId;
                    quad.visible = layer.vproperty("active");
                    layer.addMeshIndex(quad.id);
                    scene.add(quad);
                }
            }
			layer.updateGeometry = false
		}

		function getMesh(id){
			for (var i = scene.children.length - 1; i >= 0; i--) {
				var mesh = scene.children[i]
				if ( id == mesh.id)
					return mesh
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
					var mesh =  getMesh(layer.meshIndex(i)) 
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
				for(var i=0; i < layer.meshCount(); ++i){
					var mesh =  getMesh(layer.meshIndex(i)) 
					if ( mesh){
						mesh.visible = layer.vproperty("active")
						mesh.material.opacity = layer.vproperty("opacity")
					}
				}
			}
		}

        function setProperties(layer) {
            if ( layer.isDrawable && layer.isValid){
				var changedProperties = layer.changedProperties
                for(var j=0; j < changedProperties.length; ++j){
				   var property = changedProperties[j]
				   changeProperty(property, layer)
				   layer.removeFromChangedProperties(property)
				}
            }
        }
        
		onPaintGL : {
			if (scene && camera){
				if ( layermanager.needUpdate){
					updatePositions()
					layermanager.needUpdate = false
				}
				var layerList = layermanager.layerList
				for(var i=0; i < layerList.length; ++i){
					setProperties(layerList[i])
				}

				renderer.render(scene, camera);
			}
		}

		onInitializeGL: {
			layermanager.rootLayer.initSizes(canvas.width, canvas.height,true)
			var aspect = canvas.width / canvas.height
			var frustumSize = aspect >= 0 ? layermanager.rootLayer.height : layermanager.rootLayer.width
			var f = 2.0
			camera = new GL.THREE.OrthographicCamera( -frustumSize*aspect/f, frustumSize*aspect/f, frustumSize/f, -frustumSize/f , -frustumSize, frustumSize );
			var cameraPosition = layermanager.rootLayer.cameraPosition
			camera.position.set(cameraPosition.x, cameraPosition.y, frustumSize);
			camera.zoom = layermanager.rootLayer.zoomFactor
			scene = new GL.THREE.Scene();
			var pp = new GL.THREE.Vector3(cameraPosition.x, cameraPosition.y,0)
			camera.lookAt(pp);
			camera.updateProjectionMatrix();

			renderer = new GL.THREE.Canvas3DRenderer(
				   { canvas: canvas, antialias: false, devicePixelRatio: canvas.devicePixelRatio });
			renderer.setPixelRatio(canvas.devicePixelRatio);
			renderer.setSize(canvas.width, canvas.height);
			canvas.setBackgroundColor(backgroundColor);
		}
    }

}
