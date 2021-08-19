
class State {
    constructor() {
        this.canvasCtx = null;
        this.canvasCtxGL = null;
        this.imgPointer = null;
        this.maskPointer = null;
        this.imgSize = 0;
        this.graph = null;
        this.imgChannelsCount = 4;
        this.detectFace = false;
        this.selfieSegmentation = false;
        this.useBackgroundImage = false;
        this.useBackgroundColor = false;
        this.showFaceMesh = false;
        this.maskRawData = null;
        this.maskCtx = null;
        this.background = null;
    }

    getMaskRawData() {
        // console.log("getMaskRawData() this.maskRawData == null", (this.maskRawData == null));
        return this.maskRawData;
    }
};

const state = new State();

const MASK = {
    FACE_DETECTION: 1,
    SELFIE_SEGMENTATION: 2
};

// function hello() {
//     console.log(Module.helloName('alu'));
// }

const setBackgroundColor = function (event) {
    console.log("event.target.value", event.target.value);
    state.maskCtx.fillStyle = event.target.value;
    state.maskCtx.fillRect(0, 0, 640, 480);
    state.maskRawData = state.maskCtx.getImageData(0, 0, 640, 480);
    Module.HEAPU8.set(state.maskRawData.data, state.maskPointer);

}


const loadFile = function (event) {
    console.log("loading file");
    const image = document.getElementById('output');
    image.src = URL.createObjectURL(event.target.files[0]);
    if (!state.background) {
        state.background = new Image();
    }
    state.background.onload = function () {
        console.log("image loaded.");
        state.maskCtx.drawImage(state.background, 0, 0, 640, 480);
        state.maskRawData = state.maskCtx.getImageData(0, 0, 640, 480);
        Module.HEAPU8.set(state.maskRawData.data, state.maskPointer);

        console.log("state.maskRawData.data set");
    }
    state.background.src = document.getElementById("output").src; // Set source path

};


function runGraph(state, videoElem, Module) {
    // console.log("Camera:", Camera);
    if (!state.graph) {
        state.graph = new Module.GraphContainer();
    }


    const camera = new Camera(videoElem, {
        onFrame: async () => {
            state.canvasCtx.drawImage(videoElem, 0, 0, 640, 480);
            const rawData = state.canvasCtx.getImageData(0, 0, 640, 480);
            const rawDataSize = state.imgChannelsCount * rawData.width * rawData.height;
            // console.log("rawData:", rawData, "rawDataSize:", rawDataSize);
            
            state.canvasCtxOutput.clearRect(0, 0, state.canvasElementOutput.width, state.canvasElementOutput.height);
            
            console.log("state.canvasCtxGL.canvas:", state.canvasCtxGL.canvas);
            console.log("state.canvasCtxGL:", state.canvasCtxGL);
            
            state.canvasCtxOutput.drawImage(state.canvasCtxGL.canvas, 0, 0, 640, 480);

            if (!state.imgSize || state.imgSize != rawDataSize) {

                if (state.imgPointer) {
                    Module._free(state.imgPointer);
                }

                if (state.maskPointer) {
                    Module._free(state.maskPointer);
                }

                state.imgSize = rawDataSize;
                console.log("entered rawDataSize:", rawDataSize, "rawData.height:", rawData.height, "rawData.width:", rawData.width);
                state.imgPointer = Module._malloc(state.imgSize);
                state.maskPointer = Module._malloc(state.imgSize);

            }

            Module.HEAPU8.set(rawData.data, state.imgPointer);

            if (state.getMaskRawData() && (state.useBackgroundImage || state.useBackgroundColor)) {
                // Module.HEAPU8.set(state.getMaskRawData().data, state.maskPointer);
                const ret = state.graph.runWithMask(state.imgPointer, state.maskPointer, state.imgSize);
            } else {
                const ret = state.graph.runWithMask(state.imgPointer, state.imgPointer, state.imgSize);
            }

            if (state.showFaceMesh) {
                const n = state.graph.facesLandmarks.size();
                state.canvasCtxOutput.strokeStyle = "white";
                state.canvasCtxOutput.globalAlpha = 0.5;
                state.canvasCtxOutput.lineWidth = 3;

                // for (let i = 0; i < 50; i ++) {
                //     const lm = state.graph.facesLandmarks.get(i);
                //     state.canvasCtxOutput.arc(lm.x*rawData.width, lm.y*rawData.height, 5, 0, 2*Math.PI);
                //     state.canvasCtxOutput.stroke();
                // }

            }

            if (state.detectFace) {
                const n = state.graph.boundingBoxes.size();
                // console.log("n:", n, "state.graph.boundingBoxes", state.graph.boundingBoxes);

                state.canvasCtxOutput.strokeStyle = "cyan";
                state.canvasCtxOutput.lineWidth = 5;
                state.canvasCtxOutput.globalAlpha = 0.7;

                for (let i = 0; i < n; i++) {
                    // console.log("i:", i);
                    const bb = state.graph.boundingBoxes.get(i);
                    const w = bb.width * rawData.width;
                    const h = bb.height * rawData.height;
                    const x = bb.x * rawData.width;
                    const y = bb.y * rawData.height;
                    state.canvasCtxOutput.strokeRect(x, y, w, h);

                    // console.log("x:", x, "y:", y, "w:", w, "h:", h);
                }
            }
        },
        width: 640,
        height: 480
    });

    camera.start();
    // Module.runMPGraph();
}


window.onload = function () {

    const videoElement = document.getElementById('input_video');
    // videoElement.msHorizontalMirror = true;
    // videoElement.style.display = "none";

    state.canvasElement = document.getElementById('input_video_canvas');
    state.canvasElement.style.width = 640 + "px";
    state.canvasElement.style.height = 480 + "px";
    state.canvasElement.style.display = "none";
    state.canvasCtx = state.canvasElement.getContext('2d');

    state.canvasMaskElement = document.getElementById('mask_canvas');
    state.canvasMaskElement.style.width = 640 + "px";
    state.canvasMaskElement.style.height = 480 + "px";
    state.canvasMaskElement.style.display = "none";
    state.maskCtx = state.canvasMaskElement.getContext('2d');



    state.canvasElementOutput = document.getElementById('output_canvas');
    state.canvasElementOutput.style.width = 640 + "px";
    state.canvasElementOutput.style.height = 480 + "px";
    state.canvasElementOutput.style.display = "block";
    state.canvasCtxOutput = state.canvasElementOutput.getContext('2d');

    state.canvasGL = document.querySelector("#glCanvas");
    Module.canvas = state.canvasGL;

    // Initialize the GL context
    state.canvasCtxGL = state.canvasGL.getContext("webgl2", {preserveDrawingBuffer: true});

    // Only continue if WebGL is available and working
    if (state.canvasCtxGL === null) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    state.canvasGL.addEventListener(
        "webglcontextlost",
        function (e) {
            alert('WebGL context lost. You will need to reload the page.');
            e.preventDefault();
        },
        false
    );


    // Set clear color to black, fully opaque
    state.canvasCtxGL.clearColor(0.0, 1.0, 0.0, 0.5); // rgb alpha
    // Clear the color buffer with specified clear color
    state.canvasCtxGL.clear(state.canvasCtxGL.COLOR_BUFFER_BIT);


    document.getElementById("btnRunGraph").onclick = function () {
        runGraph(state, videoElement, Module);
    }
    

    document.getElementById("btnFaceMesh").onclick = function () {
        state.showFaceMesh = !state.showFaceMesh;
        if (state.showFaceMesh) {
            document.getElementById("btnFaceMesh").innerHTML = "<span>Hide Face Mesh</span>";
        } else {
            document.getElementById("btnFaceMesh").innerHTML = "<span>Show Face Mesh</span>";
        }
    }

    document.getElementById("btnFaceDetection").onclick = function () {
        state.detectFace = !state.detectFace;
    }

    document.getElementById("btnSelfieSegmentation").onclick = function () {
        state.selfieSegmentation = !state.selfieSegmentation;

        console.log("entered state.selfieSegmentation", state.selfieSegmentation);
        if (state.selfieSegmentation) {
            document.getElementById("selfie_segmentation_opts").style.display = "block";
            console.log("showing");
        } else {
            document.getElementById("selfie_segmentation_opts").style.display = "none";
        }

    }

    document.getElementById("btnSetBackground").onclick = function () {
        state.useBackgroundImage = !state.useBackgroundImage;

        if (state.useBackgroundImage) {
            document.getElementById("background_input").style.display = "block";
        } else {
            document.getElementById("background_input").style.display = "none";
        }
    }

    document.getElementById("btnSetBackgroundColor").onclick = function () {
        state.useBackgroundColor = !state.useBackgroundColor;

        console.log("state.selfieSegmentation", state.selfieSegmentation, "this.useBackgroundColor", state.useBackgroundColor);

        if (state.selfieSegmentation && state.useBackgroundColor) {
            document.getElementById("color_input").style.display = "block";
        } else {
            document.getElementById("color_input").style.display = "none";
        }
    }

    document.getElementById("file").onchange = function (event) {
        loadFile(event);
    }

    document.getElementById("favcolor").onchange = function (event) {
        console.log("favcolor onchange");
        setBackgroundColor(event);
    }

    document.getElementById("btnShowBackgroundSource").onclick = function (event) {
        if (document.getElementById("mask_canvas").style.display == "none") {
            document.getElementById("mask_canvas").style.display = "block";
            document.getElementById("btnShowBackgroundSource").innerHTML = "<span>Hide Background Source</span>";
        } else {
            document.getElementById("mask_canvas").style.display = "none";
            document.getElementById("btnShowBackgroundSource").innerHTML = "<span>Show Background Source</span>";
        }
    }

    document.getElementById("btnShowWebGL2Canvas").onclick = function(event) {
        if (document.getElementById("glCanvas").style.display == "none") {
            document.getElementById("glCanvas").style.display = "block";
            document.getElementById("glCanvas").innerHTML = "<span>Hide WebGL2 Canvas</span>";
        } else {
            document.getElementById("glCanvas").style.display = "none";
            document.getElementById("glCanvas").innerHTML = "<span>Show WebGL2 Canvas</span>";
        }
    }
}























