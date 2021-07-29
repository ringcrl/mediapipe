#include <emscripten/bind.h>
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/gpu/gl_context_internal.h"


#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"

#include "mediapipe/framework/formats/image_frame.h"

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"



#include "mediapipe/gpu/gl_simple_calculator.h"
#include "mediapipe/gpu/gl_simple_shaders.h" // GLES_VERSION_COMPAT
#include "mediapipe/gpu/shader_util.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/location_data.pb.h"

using namespace emscripten;

// namespace mediapipe {
std::string helloName(std::string name) {
  return "hello " + name;
}

/*

      // node: {
      //   calculator: "GlScalerCalculator"
      //   input_stream: "VIDEO:out1"
      //   output_stream: "VIDEO:out2"
      // }

*/

absl::Status PrintHelloWorld() {
  mediapipe::CalculatorGraphConfig config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(R"pb(
      input_stream: "in"
      output_stream: "out"
      node {
        calculator: "PassThroughCalculator"
        input_stream: "in"
        output_stream: "out1"
      }

      node {
        calculator: "PassThroughCalculator"
        input_stream: "out1"
        output_stream: "out"
      }
    )pb");

  // int num_extensions = 0;
  // glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
  // LOG(INFO) << "gl_num_extensions:" << num_extensions;
  // ERROR: Uncaught TypeError: GL.currentContext is undefined
  
  // int majorVersion;
  // glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
  // ERROR: Uncaught TypeError: GL.currentContext is undefined

  // LOG(INFO) << "GL_VERSION:" << glGetString(GL_VERSION);
  // ERROR: Uncaught TypeError: GLctx is undefined

  // LOG(INFO) << glGetString(GL_VENDOR); 
  // ERROR: Uncaught TypeError: GLctx is undefined

  mediapipe::CalculatorGraph graph;
  MP_RETURN_IF_ERROR(graph.Initialize(config));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller("out"));
  ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
  MP_RETURN_IF_ERROR(graph.SetGpuResources(gpu_resources));
  // mediapipe::GlCalculatorHelper gpu_helper;
  // gpu_helper.InitializeForTest(graph.GetGpuResources().get());

  std::vector<mediapipe::Packet> output_packets;
  
  graph.ObserveOutputStream("out", [&output_packets](const mediapipe::Packet& p) {
    output_packets.push_back(p);
    LOG(INFO) << "inside lambda func: packet.Get<std::string>():" << p.Get<std::string>();
    return absl::OkStatus();
  });

  MP_RETURN_IF_ERROR(graph.StartRun({}));

  absl::SleepFor(absl::Milliseconds(2000));

  for (int i = 0; i < 5; ++i) {
    mediapipe::Packet p = mediapipe::MakePacket<std::string>("Wasmup!").At(mediapipe::Timestamp(i));
    
    MP_RETURN_IF_ERROR(graph.AddPacketToInputStream("in", p));

    output_packets.emplace_back(p);
  }


  // Close the input stream "in".
  MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));


  // MP_RETURN_IF_ERROR(graph.WaitUntilDone());
  MP_RETURN_IF_ERROR(graph.WaitUntilIdle());


  for (const mediapipe::Packet & p: output_packets) {
    LOG(INFO) << p.Get<std::string>();
  }

  // // Get the output packets std::string.
  // while (poller.Next(&packet)) {
  // }


  MP_RETURN_IF_ERROR(graph.CloseAllPacketSources());

  MP_RETURN_IF_ERROR(absl::InvalidArgumentError("testing MP_RETURN_IF_ERROR macro"));
  // return absl::OkStatus();
}


std::string passThrough() {
  absl::Status status = PrintHelloWorld();

  if (!status.ok()) {
    LOG(WARNING) << "Unexpected error " << status;
  }

  return status.ToString();
}

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };


namespace mediapipe {


class RenderGPUBufferToCanvasCalculator: public GlSimpleCalculator {
  public:
  absl::Status GlSetup() override;
  absl::Status GlRender(const GlTexture& src,
                        const GlTexture& dst) override;
  absl::Status GlTeardown() override;

  private:
  GLuint program_ = 0;
  GLint frame_;
};

REGISTER_CALCULATOR(RenderGPUBufferToCanvasCalculator);

absl::Status RenderGPUBufferToCanvasCalculator::GlSetup() {
  LOG(INFO) << "RenderGPUBufferToCanvasCalculator::GlSetup NUM_ATTRIBUTES:" << NUM_ATTRIBUTES << " ATTRIB_VERTEX:" << ATTRIB_VERTEX << " ATTRIB_TEXTURE_POSITION:" << ATTRIB_TEXTURE_POSITION << "\n";

  const GLint attr_location[NUM_ATTRIBUTES] = {
      ATTRIB_VERTEX,
      ATTRIB_TEXTURE_POSITION,
  };

  const GLchar* attr_name[NUM_ATTRIBUTES] = {
      "position",
      "texture_coordinate",
  };

  const GLchar* frag_src = GLES_VERSION_COMPAT
      R"(
  #if __VERSION__ < 130
    #define in varying
  #endif  // __VERSION__ < 130

  #ifdef GL_ES
    #define fragColor gl_FragColor
    precision highp float;
  #else
    #define lowp
    #define mediump
    #define highp
    #define texture2D texture
    out vec4 fragColor;
  #endif  // defined(GL_ES)

    in vec2 sample_coordinate;
    uniform sampler2D video_frame;
    const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      vec4 color = texture2D(video_frame, sample_coordinate);
      // float luminance = dot(color.rgb, W);
      // fragColor.rgb = vec3(luminance);
      // fragColor.rgb = vec3(0.0, 0.0, 1.0);
      fragColor.rgb = color.rgb;

      fragColor.a = 1.0;
      // fragColor.a = color.a;
    }

  )";

  // Creates a GLSL program by compiling and linking the provided shaders.
  // Also obtains the locations of the requested attributes.
  auto glStatus = GlhCreateProgram(kBasicVertexShader, frag_src, NUM_ATTRIBUTES,
                   (const GLchar**)&attr_name[0], attr_location, &program_);
  
  if (glStatus != GL_TRUE) {
    LOG(ERROR) << "GlhCreateProgram failed";
  } else {
    LOG(INFO) << "GlhCreateProgram success";
  }
  
  RET_CHECK(program_) << "Problem initializing the program.";
  frame_ = glGetUniformLocation(program_, "video_frame");
  
  return absl::OkStatus();
}

absl::Status RenderGPUBufferToCanvasCalculator::GlRender(const GlTexture& src, const GlTexture& dst) {
  static const GLfloat square_vertices[] = {
      -1.0f, -1.0f,  // bottom left
      1.0f,  -1.0f,  // bottom right
      -1.0f, 1.0f,   // top left
      1.0f,  1.0f,   // top right
  };
  static const GLfloat texture_vertices[] = {
      0.0f, 1.0f,  // top left
      1.0f, 1.0f,  // top right
      0.0f, 0.0f,  // bottom left
      1.0f, 0.0f,  // bottom right
  };

  glBindFramebuffer(GL_FRAMEBUFFER, 0); // binding to canvas

  // program
  glUseProgram(program_);
  glUniform1i(frame_, 1);

  // vertex storage
  GLuint vbo[2];
  glGenBuffers(2, vbo);
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo 0
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), square_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(ATTRIB_VERTEX);
  glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, nullptr);

  // vbo 1
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), texture_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
  glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, 0, 0, nullptr);

  // draw
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0); // glBindFramebuffer after draw does not work


  // cleanup
  glDisableVertexAttribArray(ATTRIB_VERTEX);
  glDisableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(2, vbo);


  return absl::OkStatus();  
}

absl::Status RenderGPUBufferToCanvasCalculator::GlTeardown() {
  if (program_) {
    glDeleteProgram(program_);
    program_ = 0;
  }

  return absl::OkStatus();
}

}  // namespace mediapipe

class BoundingBox {
  public:
  float x, y, width, height;

  BoundingBox(float x, float y, float w, float h) {
    this->x = x;
    this->y = y; 
    this->width = w;
    this->height = h;
  }

  BoundingBox() {}
};


class GraphContainer {
  public:
  mediapipe::CalculatorGraph graph;
  bool isGraphInitialized = false;
  mediapipe::GlCalculatorHelper gpu_helper;
  int w = 0;
  int direction = 1;
  int runCounter;
  std::vector<mediapipe::Packet> output_packets;

  uint8* data;

  int* prvTemp;
  std::vector<BoundingBox> boundingBoxes;
  
  std::string graphConfigWithRender = R"pb(
        input_stream: "input_video"
        output_stream: "output_video"
        output_stream: "face_detections"
        max_queue_size: 5

      

        node: {
          calculator: "ImageFrameToGpuBufferCalculator"
          input_stream: "input_video"
          output_stream: "output_video_gpubuffer"
        }
      
        # Converts RGB images into luminance images, still stored in RGB format.
        # Subgraph that detects faces.
        node {
          calculator: "FaceDetectionShortRangeGpu"
          input_stream: "IMAGE:output_video_gpubuffer"
          output_stream: "DETECTIONS:face_detections"
        }

        node: {
          calculator: "RenderGPUBufferToCanvasCalculator"
          input_stream: "VIDEO:output_video_gpubuffer"
          output_stream: "VIDEO:output_video"
        }
      )pb";
  

  absl::Status setupGraph() {

    mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(graphConfigWithRender);

    MP_RETURN_IF_ERROR(graph.Initialize(config));
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                    graph.AddOutputStreamPoller("output_video"));
    // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller pollerDetections,
    //                 graph.AddOutputStreamPoller("detections"));
    ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
    MP_RETURN_IF_ERROR(graph.SetGpuResources(gpu_resources));
    gpu_helper.InitializeForTest(graph.GetGpuResources().get());
    graph.ObserveOutputStream("output_video", [this](const mediapipe::Packet& p) {
      // LOG(INFO) << "observing packet in output_video output stream";
      // LOG(INFO) << "inside lambda func: packet.Get<std::string>():" << p.Get<std::string>();
      return absl::OkStatus();
    });

    graph.ObserveOutputStream("face_detections", [this](const mediapipe::Packet& p) {
      const auto& detections = p.Get<std::vector<mediapipe::Detection>>();

      const int n = detections.size();

      this->boundingBoxes.resize(n);
      float xmin, ymin, width, height;

      for (int i = 0; i < n; i ++) {
        mediapipe::LocationData loc = detections[i].location_data();

        if (loc.format() == mediapipe::LocationData::RELATIVE_BOUNDING_BOX) {
          auto boundingBox = loc.relative_bounding_box();
          xmin = boundingBox.xmin();
          ymin = boundingBox.ymin();
          width = boundingBox.width();
          height = boundingBox.height();
          this->boundingBoxes[i].x = xmin;
          this->boundingBoxes[i].y = ymin;
          this->boundingBoxes[i].width = width;
          this->boundingBoxes[i].height = height;
        }

        LOG(INFO) <<  " xmin:" << xmin << " ymin:" << ymin << " width:" << width << " height:" << height;
      }

      LOG(INFO) << "detections size:" << n;

      // for (const mediapipe::Detection & d: detections) {
      //   LOG(INFO) << "has_detection_id:" << d.has_detection_id(); // << " detection_id:" << d.detection_id() << " score:" << d.score();
      // }

      return absl::OkStatus();
    });


    MP_RETURN_IF_ERROR(graph.StartRun({}));

    return absl::OkStatus();
  }

  absl::Status init() {
    isGraphInitialized = false;
    w = 0;
    direction = 1;
    runCounter = 0;
    prvTemp = nullptr;

    FILE* ret = freopen("assets/in.txt", "r", stdin);
    if (ret == nullptr) {
      LOG(ERROR) << "could not open assets/in.txt";
    }
    int n;
    while (std::cin >> n) {
      LOG(INFO) << "From file: " << n;
    }

    return this->setupGraph();
  }

  GraphContainer(uint32 maxWidth, uint32 maxHeight) {  
    data = (uint8*)malloc(4*480*640);
    
    absl::Status status = this->init();
    if (!status.ok()) {
      LOG(ERROR) << status;
    }
  }

  GraphContainer() {  
    data = (uint8*)malloc(4*480*640);

    absl::Status status = this->init();
    if (!status.ok()) {
      LOG(ERROR) << status;
    }
  }


  absl::Status webglCanvasDraw(uint8* imgData, int imgSize) {

    int* temp = (int *) malloc(5000);
    if (prvTemp == nullptr) prvTemp = temp;
    printf("temp: %d, change: %d \n", static_cast<int *>(temp), (temp - prvTemp));
    prvTemp = temp;
    free(temp);


    uint8* imgPtr = imgData;
    
    // int ptr = 0;

    w += direction;
    if (w == 500 || w == 0) direction = -direction;

    // LOG(INFO) << "w:" << w;
    LOG(INFO) << "imgSize:" << imgSize << "(4*480*640):" << (4*480*640);

    for (int ptr = 0; ptr < imgSize; ptr += 4) {
      // for (int j = 0;j < 4; j ++) {  
    // for (int j = 0; j < 480; j ++) {
    // // for (int j = 479; j >= 0; j --) {
    //   for (int k = 0; k < 640; k ++) {
        // rgba
        data[ptr] = (255*w) / 500;
        imgPtr ++;
        data[ptr + 1] = *imgPtr;
        imgPtr ++;
        data[ptr + 2] = *imgPtr;
        imgPtr ++;
        data[ptr + 3] = *imgPtr; 
        imgPtr ++;
        // ptr += 4;
      // }
    }

    auto imageFrame =
        absl::make_unique<mediapipe::ImageFrame>(mediapipe::ImageFormat::SRGBA, 640, 480,
                                    mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
    int img_data_size = 640 * 480 * 4;
    std::memcpy(imageFrame->MutablePixelData(), data,
                img_data_size);
    // if (!(graph_.AddPacketToInputStream(
    //           "input_video", Adopt(imageFrame.release())
    //               .At(currentTimestamp()))) {
    //                 LOG(ERROR) << "could not add packet to graph";
                  // }

    // mediapipe::ImageFrame * imageFrame = new mediapipe::ImageFrame(
    //   mediapipe::ImageFormat::SRGBA, 
    //   640, 
    //   480, 
    //   mediapipe::ImageFrame::kGlDefaultAlignmentBoundary
    // );

    // imageFrame->CopyPixelData(
    //   mediapipe::ImageFormat::SRGBA,
    //   640,
    //   480, 
    //   data,
    //   mediapipe::ImageFrame::kGlDefaultAlignmentBoundary
    // );

    // imageFrame->AdoptPixelData(
    //   mediapipe::ImageFormat::SRGBA,
    //   640,
    //   480, 
    //   0,
    //   data
    // );
    size_t frame_timestamp_us = runCounter * 1e6;
    runCounter ++;

    MP_RETURN_IF_ERROR(          
      graph.AddPacketToInputStream(
        "input_video",
        mediapipe::Adopt(
          imageFrame.release()
        ).At(
          mediapipe::Timestamp(frame_timestamp_us)
        )
      )
    ); 

    MP_RETURN_IF_ERROR(
      gpu_helper.RunInGlContext(
        [this]() -> absl::Status {
          
          glFlush();
          
          MP_RETURN_IF_ERROR(
            this->graph.WaitUntilIdle()
          );

          return absl::OkStatus();
        }
      )
    );

    // delete imageFrame;
    // delete data;
    
    // MP_RETURN_IF_ERROR(graph.WaitUntilDone());
    return absl::OkStatus();
  }

  // std::string runMPGraph(uint8* imgData, int imgSize) {
  // std::vector<BoundingBox> run(uintptr_t imgData, int imgSize) {
  std::string run(uintptr_t imgData, int imgSize) {
    // absl::Status status = this->webglCanvasDraw(imgData, imgSize);

    absl::Status status = this->webglCanvasDraw(reinterpret_cast<uint8*>(imgData), imgSize);

    if (!status.ok()) {
      LOG(WARNING) << "Unexpected error " << status;
    }

    return status.ToString();
  }

  absl::Status cleanGraph() {
    MP_RETURN_IF_ERROR(graph.CloseInputStream("input_video"));
    MP_RETURN_IF_ERROR(graph.CloseAllPacketSources());
    return absl::OkStatus();
  }

  ~GraphContainer() {
    absl::Status stat = cleanGraph();
    if (!stat.ok()) {
      LOG(ERROR) << stat;
    }
  }
};

// ------------------------ OLD START -------------------------------

absl::Status webglCanvasDrawBasic(uint8* imgData, int imgSize) {
  mediapipe::CalculatorGraph graph;
  mediapipe::GlCalculatorHelper gpu_helper;
  // LOG(INFO) << "NUM_ATTRIBUTES:" << NUM_ATTRIBUTES << " ATTRIB_VERTEX:" << ATTRIB_VERTEX << " ATTRIB_TEXTURE_POSITION:" << ATTRIB_TEXTURE_POSITION << "\n";
    mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(R"pb(
        input_stream: "input_video"
        output_stream: "output_video"

        # Converts RGB images into luminance images, still stored in RGB format.
        
        node: {
          calculator: "ImageFrameToGpuBufferCalculator"
          input_stream: "input_video"
          output_stream: "output_video_gpubuffer"
        }
        
        node: {
          calculator: "RenderGPUBufferToCanvasCalculator"
          input_stream: "VIDEO:output_video_gpubuffer"
          output_stream: "VIDEO:output_video"
        }
      )pb");

    MP_RETURN_IF_ERROR(graph.Initialize(config));
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                    graph.AddOutputStreamPoller("output_video"));
    ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
    MP_RETURN_IF_ERROR(graph.SetGpuResources(gpu_resources));
    gpu_helper.InitializeForTest(graph.GetGpuResources().get());

    std::vector<mediapipe::Packet> output_packets;

    graph.ObserveOutputStream("output_video", [&output_packets](const mediapipe::Packet& p) {
    // output_packets.push_back(p);
      LOG(INFO) << "observing packet in output_video output stream";
      // LOG(INFO) << "inside lambda func: packet.Get<std::string>():" << p.Get<std::string>();
      return absl::OkStatus();
    });
    
    MP_RETURN_IF_ERROR(graph.StartRun({}));
  
  for (int i = 1; i <= 450; ++i) {
    
    // absl::SleepFor(absl::Milliseconds(100));

    uint8* data = new uint8[4*480*640];
    int ptr = 0;

    for (int j = 0; j < 480; j ++) {
      for (int k = 0; k < 640; k ++) {
        ptr += 4;
        // rgba
        data[ptr] = (255*i) / 500;
        data[ptr + 1] = 125;
        data[ptr + 2] = 125;
        data[ptr + 3] = 1; 
      }
    }

    mediapipe::ImageFrame * imageFrame = new mediapipe::ImageFrame(
      mediapipe::ImageFormat::SRGBA, 
      640, 
      480, 
      mediapipe::ImageFrame::kGlDefaultAlignmentBoundary
    );

    imageFrame->CopyPixelData(
      mediapipe::ImageFormat::SRGBA,
      640,
      480, 
      data,
      mediapipe::ImageFrame::kGlDefaultAlignmentBoundary
    );

    // imageFrame->AdoptPixelData(
    //   mediapipe::ImageFormat::SRGBA,
    //   640,
    //   480, 
    //   0,
    //   data
    // );
    size_t frame_timestamp_us = i * 1e6;

    MP_RETURN_IF_ERROR(          
      graph.AddPacketToInputStream(
        "input_video",
        mediapipe::Adopt(
          imageFrame
        ).At(
          mediapipe::Timestamp(frame_timestamp_us)
        )
      )
    ); 

    MP_RETURN_IF_ERROR(
      gpu_helper.RunInGlContext(
        [&graph]() -> absl::Status {
          
          glFlush();
          
          MP_RETURN_IF_ERROR(
            graph.WaitUntilIdle()
          );

          return absl::OkStatus();
        }
      )
    );
  }

  // Close the input stream "in".
  // MP_RETURN_IF_ERROR(graph.CloseInputStream("input_video"));


  // MP_RETURN_IF_ERROR(graph.WaitUntilDone());

  for (const mediapipe::Packet & p: output_packets) {
    LOG(INFO) << p.Get<std::string>();
  }

  MP_RETURN_IF_ERROR(graph.CloseAllPacketSources());

  // MP_RETURN_IF_ERROR(absl::InvalidArgumentError("testing MP_RETURN_IF_ERROR macro"));
  return absl::OkStatus();
}

// std::string runMPGraph(uint8* imgData, int imgSize) {
std::string runMPGraph(uintptr_t imgData, int imgSize) {
  // absl::Status status = webglCanvasDraw(imgData, imgSize);
  absl::Status status = webglCanvasDrawBasic(reinterpret_cast<uint8*>(imgData), imgSize);

  if (!status.ok()) {
    LOG(WARNING) << "Unexpected error " << status;
  }

  return status.ToString();
}

// ------------------------ OLD END -------------------------------


int main() {}

EMSCRIPTEN_BINDINGS(Hello_World_Simple) {
  // function("PrintHelloWorld", &mediapipe::PrintHelloWorld);
  function("helloName", &helloName);
//   function("helloName", &helloName);
  function("passThrough", &passThrough);
  function("runMPGraph", &runMPGraph, allow_raw_pointers());
  class_<GraphContainer>("GraphContainer")
    .constructor()
    .constructor<int, int>()
    .function("run", &GraphContainer::run)
    .property("boundingBoxes", &GraphContainer::boundingBoxes)
    ;
  class_<BoundingBox>("BoundingBox")
    // .constructor<float, float, float, float>()
    .property("x", &BoundingBox::x)
    .property("y", &BoundingBox::y)
    .property("width", &BoundingBox::width)
    .property("height", &BoundingBox::height)
    ;
  register_vector<BoundingBox>("vector<BoundingBox>");
  
}