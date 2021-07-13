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

/*
If we want to be able to repeat it and potentially run multiple frames, then we can just get rid of the "CloseInputStream" call entirely, and can use "graph.WaitUntilIdle()" instead of "graph.WaitUntilDone"
it's fine to just leave input streams open forever, and leave the graph kinda "perpetually running"...  when we're all done with it, we can then just call graph.Close() and it should close and clean up everything at once, so it's a lot simpler/easier that way
*/

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


class WebGLCalculator: public GlSimpleCalculator {
  public:
  absl::Status GlSetup() override;
  absl::Status GlRender(const GlTexture& src,
                        const GlTexture& dst) override;
  absl::Status GlTeardown() override;

  private:
  GLuint program_ = 0;
  GLint frame_;
};

REGISTER_CALCULATOR(WebGLCalculator);

absl::Status WebGLCalculator::GlSetup() {
  LOG(INFO) << "WebGLCalculator::GlSetup NUM_ATTRIBUTES:" << NUM_ATTRIBUTES << " ATTRIB_VERTEX:" << ATTRIB_VERTEX << " ATTRIB_TEXTURE_POSITION:" << ATTRIB_TEXTURE_POSITION << "\n";

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
      fragColor.rgb = vec3(0.0, 0.0, 1.0);
      fragColor.a = color.a;
    }

  )";

  GlhCreateProgram(kBasicVertexShader, frag_src, NUM_ATTRIBUTES,
                   (const GLchar**)&attr_name[0], attr_location, &program_);
  RET_CHECK(program_) << "Problem initializing the program.";
  frame_ = glGetUniformLocation(program_, "video_frame");
  
  return absl::OkStatus();
}

absl::Status WebGLCalculator::GlRender(const GlTexture& src, const GlTexture& dst) {
  static const GLfloat square_vertices[] = {
      -1.0f, -1.0f,  // bottom left
      1.0f,  -1.0f,  // bottom right
      -1.0f, 1.0f,   // top left
      1.0f,  1.0f,   // top right
  };
  static const GLfloat texture_vertices[] = {
      0.0f, 0.0f,  // bottom left
      1.0f, 0.0f,  // bottom right
      0.0f, 1.0f,  // top left
      1.0f, 1.0f,  // top right
  };

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

  // cleanup
  glDisableVertexAttribArray(ATTRIB_VERTEX);
  glDisableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(2, vbo);


  return absl::OkStatus();  
}

absl::Status WebGLCalculator::GlTeardown() {
  if (program_) {
    glDeleteProgram(program_);
    program_ = 0;
  }

  return absl::OkStatus();
}

}  // namespace mediapipe


absl::Status webglCanvasDraw() {

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
        calculator: "WebGLCalculator"
        input_stream: "VIDEO:output_video_gpubuffer"
        output_stream: "VIDEO:output_video"
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
                   graph.AddOutputStreamPoller("output_video"));
  ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
  MP_RETURN_IF_ERROR(graph.SetGpuResources(gpu_resources));
  // mediapipe::GlCalculatorHelper gpu_helper;
  // gpu_helper.InitializeForTest(graph.GetGpuResources().get());

  std::vector<mediapipe::Packet> output_packets;
  
  graph.ObserveOutputStream("output_video", [&output_packets](const mediapipe::Packet& p) {
    // output_packets.push_back(p);
    LOG(INFO) << "observing packet in output_video output stream";
    // LOG(INFO) << "inside lambda func: packet.Get<std::string>():" << p.Get<std::string>();
    return absl::OkStatus();
  });

  MP_RETURN_IF_ERROR(graph.StartRun({}));

  absl::SleepFor(absl::Milliseconds(200));

  
  
  for (int i = 0; i < 5; ++i) {
    MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
      "input_video",
      mediapipe::Adopt(new mediapipe::ImageFrame(mediapipe::ImageFormat::SRGBA, 1, 2)).At(mediapipe::Timestamp(i))));
  }


  // Close the input stream "in".
  MP_RETURN_IF_ERROR(graph.CloseInputStream("input_video"));


  // MP_RETURN_IF_ERROR(graph.WaitUntilDone());
  MP_RETURN_IF_ERROR(graph.WaitUntilIdle());


  for (const mediapipe::Packet & p: output_packets) {
    LOG(INFO) << p.Get<std::string>();
  }

  MP_RETURN_IF_ERROR(graph.CloseAllPacketSources());

  MP_RETURN_IF_ERROR(absl::InvalidArgumentError("testing MP_RETURN_IF_ERROR macro"));
  // return absl::OkStatus();
}


std::string runMPGraph() {
  absl::Status status = webglCanvasDraw();

  if (!status.ok()) {
    LOG(WARNING) << "Unexpected error " << status;
  }

  return status.ToString();
}



// } // namespace mediapipe


int main() {}

EMSCRIPTEN_BINDINGS(Hello_World_Simple) {
  // function("PrintHelloWorld", &mediapipe::PrintHelloWorld);
  function("helloName", &helloName);
//   function("helloName", &helloName);
  function("passThrough", &passThrough);
  function("runMPGraph", &runMPGraph);
  
}