
#ifndef MEDIAPIPE_GPUBUFFER_TO_CANVAS_RENDERER_H_
#define MEDIAPIPE_GPUBUFFER_TO_CANVAS_RENDERER_H_

#include <utility>  // for declval

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gl_simple_shaders.h" // GLES_VERSION_COMPAT
#include "mediapipe/gpu/gl_simple_calculator.h"
#include "mediapipe/gpu/shader_util.h" // use of undeclared identifier 'GlhCreateProgram'


namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

class RenderGPUBufferToCanvasCalculator: public CalculatorBase {
  public:
  RenderGPUBufferToCanvasCalculator() : initialized_(false) {}
  RenderGPUBufferToCanvasCalculator(const RenderGPUBufferToCanvasCalculator&) = delete;
  RenderGPUBufferToCanvasCalculator& operator=(const RenderGPUBufferToCanvasCalculator&) = delete;
  ~RenderGPUBufferToCanvasCalculator() override = default;

  static absl::Status GetContract(CalculatorContract* cc);
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;
  absl::Status Close(CalculatorContext* cc) override;

  absl::Status GlBind() { return absl::OkStatus(); }

  void GetOutputDimensions(int src_width, int src_height,
                                   int* dst_width, int* dst_height) {
    *dst_width = src_width;
    *dst_height = src_height;
  }

  virtual GpuBufferFormat GetOutputFormat() { return GpuBufferFormat::kBGRA32; } 
  
  absl::Status GlSetup();
  absl::Status GlRender(const GlTexture& src,
                        const GlTexture& dst);
  absl::Status GlTeardown();

 protected:
  template <typename F>
  auto RunInGlContext(F&& f)
      -> decltype(std::declval<GlCalculatorHelper>().RunInGlContext(f)) {
    return helper_.RunInGlContext(std::forward<F>(f));
  }

  GlCalculatorHelper helper_;
  bool initialized_;
  
  private:
  GLuint program_ = 0;
  GLint frame_;
};

REGISTER_CALCULATOR(RenderGPUBufferToCanvasCalculator);

} // namespace mediapipe


#endif  // MEDIAPIPE_GPUBUFFER_TO_CANVAS_RENDERER_H_
