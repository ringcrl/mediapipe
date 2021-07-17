

function hello() {
    console.log(Module.helloName('alu'));
}

function runGraph(videoElem, canvasCtx, Module) {
    // console.log("Camera:", Camera);
    const camera = new Camera(videoElem, {
        onFrame: async () => {
            // const rawData = canvasCtx.getImageData(0, 0, 640, 480);
            // console.log("rawData:", rawData);
        },
        width: 640,
        height: 480
    });

    camera.start();
    Module.runMPGraph();
}


window.onload = function() {
    const videoElement =
    document.getElementById('input_video');
    // videoElement.style.display = "none";
    
    
    const canvasElement =
    document.getElementById('output_canvas');
    canvasElement.style.width = 640 + "px";
    canvasElement.style.height = 480 + "px";
    canvasElement.style.display = "none";
    
    const canvasCtx = canvasElement.getContext('2d');
    
    const canvasGL = document.querySelector("#glCanvas");
    canvasGL.addEventListener(
        "webglcontextlost", 
        function(e) { 
            alert('WebGL context lost. You will need to reload the page.'); 
            e.preventDefault(); 
        }, 
        false
        );
        
    Module.canvas = canvasGL;

    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    const gl = canvas.getContext("webgl2");

    // Only continue if WebGL is available and working
    if (gl === null) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    // Set clear color to black, fully opaque
    gl.clearColor(0.0, 1.0, 0.0, 0.5); // rgb alpha
    // Clear the color buffer with specified clear color
    gl.clear(gl.COLOR_BUFFER_BIT);

    document.getElementById("btnRunGraph").onclick = function() {
        runGraph(videoElement, canvasCtx, Module);
    }
}


