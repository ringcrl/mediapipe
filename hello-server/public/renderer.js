
class State {
    constructor() {
        this.imgPointer = null;
        this.imgSize = 0;
        this.graph = null;
        this.imgChannelsCount = 4;
    }
};

let state = new State();


function hello() {
    console.log(Module.helloName('alu'));
}

function runGraphExp(state, videoElem, canvasCtx, Module) {
    if (!state.graph) {
        state.graph = new Module.GraphContainer();
    }
    const img = new Image();   // Create new img element
    img.onload = function() {
        console.log("image loaded.");
    }
    img.src = 'res/demo.jpg'; // Set source path

    const interval = setInterval(function() {
        canvasCtx.drawImage(img, 0, 0, 640, 480);
        const rawData = canvasCtx.getImageData(0, 0, 640, 480);
        const rawDataSize = state.imgChannelsCount * rawData.width*rawData.height;
        // console.log("rawData:", rawData, "rawDataSize:", rawDataSize);
    
        if (!state.imgSize || state.imgSize != rawDataSize) {
            
            if (state.imgPointer) {
                Module._free(state.imgPointer);
            }
            
            state.imgSize = rawDataSize;
            console.log("entered rawDataSize:", rawDataSize, "rawData.height:", rawData.height, "rawData.width:", rawData.width);
            state.imgPointer = Module._malloc(state.imgSize);
        }
    
        Module.HEAPU8.set(rawData.data, state.imgPointer);
        const ret = state.graph.run(state.imgPointer, state.imgSize);

    }, 200);
    
    console.log("interval:", interval);
}

function runGraph(state, videoElem, canvasCtx, Module) {
    // console.log("Camera:", Camera);
    if (!state.graph) {
        state.graph = new Module.GraphContainer();
    }
    
        const camera = new Camera(videoElem, {
            onFrame: async () => {
                canvasCtx.drawImage(videoElem, 0, 0, 640, 480);
                const rawData = canvasCtx.getImageData(0, 0, 640, 480);
                const rawDataSize = state.imgChannelsCount * rawData.width*rawData.height;
                // console.log("rawData:", rawData, "rawDataSize:", rawDataSize);
    
                if (!state.imgSize || state.imgSize != rawDataSize) {
                    
                    if (state.imgPointer) {
                        Module._free(state.imgPointer);
                    }
                    
                    state.imgSize = rawDataSize;
                    console.log("entered rawDataSize:", rawDataSize, "rawData.height:", rawData.height, "rawData.width:", rawData.width);
                    state.imgPointer = Module._malloc(state.imgSize);
                }
    
                Module.HEAPU8.set(rawData.data, state.imgPointer);
                const ret = state.graph.run(state.imgPointer, state.imgSize);
            },
            width: 640,
            height: 480
        });
    
        camera.start();
    // Module.runMPGraph();
}


window.onload = function() {  
    const videoElement =
        document.getElementById('input_video');
    // videoElement.msHorizontalMirror = true;
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
        // runGraph(state, videoElement, canvasCtx, Module);
        runGraphExp(state, videoElement, canvasCtx, Module);
    }
}


