#include "renderer.h"

namespace mediapipe{
absl::Status RenderGPUBufferToCanvasCalculator::GetContract(CalculatorContract* cc) {
  TagOrIndex(&cc->Inputs(), "VIDEO", 0).Set<GpuBuffer>();
  TagOrIndex(&cc->Outputs(), "VIDEO", 0).Set<GpuBuffer>();
  // Currently we pass GL context information and other stuff as external
  // inputs, which are handled by the helper.
  return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status RenderGPUBufferToCanvasCalculator::Open(CalculatorContext* cc) {
  // Inform the framework that we always output at the same timestamp
  // as we receive a packet at.
  cc->SetOffset(mediapipe::TimestampDiff(0));

  // Let the helper access the GL context information.
  return helper_.Open(cc);
}

absl::Status RenderGPUBufferToCanvasCalculator::Process(CalculatorContext* cc) {
  return RunInGlContext([this, cc]() -> absl::Status {
    const auto& input = TagOrIndex(cc->Inputs(), "VIDEO", 0).Get<GpuBuffer>();
    if (!initialized_) {
      MP_RETURN_IF_ERROR(GlSetup());
      initialized_ = true;
    }

    auto src = helper_.CreateSourceTexture(input);
    int dst_width;
    int dst_height;
    GetOutputDimensions(src.width(), src.height(), &dst_width, &dst_height);
    auto dst = helper_.CreateDestinationTexture(dst_width, dst_height,
                                                GetOutputFormat());

    helper_.BindFramebuffer(dst);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(src.target(), src.name());

    MP_RETURN_IF_ERROR(GlBind());
    // Run core program.
    MP_RETURN_IF_ERROR(GlRender(src, dst));

    glBindTexture(src.target(), 0);

    glFlush();

    auto output = dst.GetFrame<GpuBuffer>();

    src.Release();
    dst.Release();

    TagOrIndex(&cc->Outputs(), "VIDEO", 0)
        .Add(output.release(), cc->InputTimestamp());

    return absl::OkStatus();
  });
}

absl::Status RenderGPUBufferToCanvasCalculator::Close(CalculatorContext* cc) {
  return RunInGlContext([this]() -> absl::Status { return GlTeardown(); });
}

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

      // fragColor.a = 1.0;
      fragColor.a = color.a;
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
