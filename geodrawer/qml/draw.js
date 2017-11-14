Qt.include("three.js")

var camera, scene, renderer,mesh,material;

function initializeGL(canvas) {
    var frustumSize = 2000
    var aspect = canvas.width / canvas.height
   camera = new THREE.OrthographicCamera( frustumSize * aspect / - 2, frustumSize * aspect / 2, frustumSize / 2, frustumSize / - 2, 1, 10000 );
  //  camera = new THREE.PerspectiveCamera(45, canvas.width / canvas.height, 1, 1000);
       camera.position.set(0,0, 10000);
    scene = new THREE.Scene();
    camera.lookAt(scene.position);

    var geometry = new THREE.BufferGeometry();
    // create a simple square shape. We duplicate the top left and bottom right
    // vertices because each vertex needs to appear once per triangle.
    var vertices = new Float32Array( [
        -80, -80,  80,
         80, -80,  80,
         80,  80,  80,

         80,  80,  80,
        -80,  80,  80,
        -80, -80,  80
    ] );

    var colors = new Float32Array( [
        1, 0,  1,
         1, 0,  0.5,
         0,  0,  0.5,

         1,  0,  1,
        0,  0,  0.5,
        0, 1,  0.5
    ] );

    // itemSize = 3 because there are 3 values (components) per vertex

    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
     geometry.addAttribute( 'color', new THREE.BufferAttribute( colors, 3 ) );
    //material = new THREE.MeshBasicMaterial( { color: 0x000000, opacity: 0.3,transparent:true } );
    material = new THREE.MeshBasicMaterial({ vertexColors: THREE.VertexColors });
    mesh = new THREE.Mesh( geometry, material );


       scene.add( mesh );


       renderer = new THREE.Canvas3DRenderer(
                   { canvas: canvas, antialias: true, devicePixelRatio: canvas.devicePixelRatio });
       renderer.setPixelRatio(canvas.devicePixelRatio);
       renderer.setSize(canvas.width, canvas.height);
       setBackgroundColor("#00FCFCFC");
}

function setBackgroundColor(backgroundColor) {
    var str = ""+backgroundColor;
    var color = parseInt(str.substring(1), 16);
    if (renderer)
        renderer.setClearColor(color);
}

function paintGL(canvas) {
    renderer.render(scene, camera);
}
