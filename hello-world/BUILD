package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")

DEFAULT_EMSCRIPTEN_LINKOPTS = [
#    "-flto",                            # Specify lto (has to be set on for compiler as well), to disable use: -fno-lto
    "--bind",                           # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "--closure 1",                      # Run the closure compiler
    "-s MALLOC=emmalloc",               # Switch to using the much smaller implementation
    "-s ALLOW_MEMORY_GROWTH=0",         # Our example doesn't need memory growth
    "-s USE_PTHREADS=1",                # Disable pthreads
    "-s ASSERTIONS=0",                  # Turn off assertions
    "-s EXPORT_ES6=1",                  # Export as es6 module, used for rollup
    "-s MODULARIZE=1",                  # Allows us to manually invoke the initializatio of wasm
    "-s EXPORT_NAME=createModule",      # Not used, but good to specify
    "-s USE_ES6_IMPORT_META=0",         # Disable loading from import meta since we use rollup
    "-s SINGLE_FILE=1",                 # Pack all webassembly into base64
    "-s DISABLE_EXCEPTION_CATCHING=1",  # Disable all exception catching
    "-s NODEJS_CATCH_EXIT=0",           # We don't have a 'main' so disable exit() catching
]

WASM_LINKOPTS = [
    "-s WASM=1",                    # Specify wasm output
]

# notes
# warning: Link with `-s LLD_REPORT_UNDEFINED` to get more information on undefined symbols

cc_binary(
    name = "hello-world-wasm.js",
    srcs = ["main.cpp"],
    linkopts = ["-s USE_PTHREADS=0", "-s ALLOW_MEMORY_GROWTH=0", "-s ERROR_ON_UNDEFINED_SYMBOLS=1", "--export-all", "--no-entry", "-s ASSERTIONS=0", "--fatal-warnings"],
#    linkopts = DEFAULT_EMSCRIPTEN_LINKOPTS + WASM_LINKOPTS,
    deps = [
        "//hello-world/cpp:popen",
        "//hello-world/cpp:greet",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/core:gate_calculator_cc_proto",
        "//mediapipe/framework:counter",
        "//mediapipe/framework:port",
        "//mediapipe/framework/port:integral_types",
        "//third_party:glog",
        "@com_google_absl//absl/time",
        "//mediapipe/framework/port:logging",
        "//mediapipe/framework/deps:map_util",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework:counter_factory",
        "//mediapipe/framework:calculator_state",
        "//mediapipe/framework:calculator_context",
        "//mediapipe/framework:calculator_base",
        "//mediapipe/framework:calculator_context_manager",
        "//mediapipe/framework:input_side_packet_handler",
        "//mediapipe/framework:input_stream_handler",
        "//mediapipe/framework:input_stream_manager",
        "//mediapipe/framework:input_stream_shard",
        "//mediapipe/framework:packet",
        "//mediapipe/framework:packet_set",
        "//mediapipe/framework:packet_type",
        "//mediapipe/framework:timestamp",
        "//mediapipe/framework:validated_graph_config",
        "//mediapipe/framework:calculator_cc_proto",
        "//mediapipe/framework/deps:registration",
        "//mediapipe/framework/port:core_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:source_location",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/profiler:graph_profiler",
        "//mediapipe/framework/tool:name_util",
        "//mediapipe/framework/tool:status_util",
        "//mediapipe/framework/tool:tag_map",
        "//mediapipe/framework/tool:validate_name",
        "//mediapipe/gpu:graph_support",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "//mediapipe/framework/stream_handler:default_input_stream_handler_cc_proto",
        "//mediapipe/framework:collection",
        "//mediapipe/framework:collection_item_id",
        "//mediapipe/framework:output_stream_handler",
        "//mediapipe/framework:output_stream_manager",
        "//mediapipe/framework:output_stream_shard",
        "//mediapipe/framework:stream_handler_cc_proto",        # solved by adding hello-world to mediapipe_internal
        "//mediapipe/framework:mediapipe_options_cc_proto",     # solved by adding hello-world to mediapipe_internal
        "//mediapipe/framework/stream_handler:default_input_stream_handler",    # popen
        "//mediapipe/framework/stream_handler:in_order_output_stream_handler",  # popen
        "//mediapipe/framework:calculator_node",                                # popen recursed from here
        "//mediapipe/framework:calculator_graph",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:split_vector_calculator",
        "//mediapipe/framework/api2:node",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:rect_cc_proto",
        "//mediapipe/gpu:gpu_origin_cc_proto",
        "//mediapipe/calculators/tensor:image_to_tensor_converter",
        "//mediapipe/calculators/tensor:image_to_tensor_utils",
        "//mediapipe/framework/formats:image",
        "//mediapipe/framework/formats:image_format_cc_proto",
        "//mediapipe/framework/formats:tensor",
        "//mediapipe/framework/port:statusor",
        "//mediapipe/calculators/tensor:image_to_tensor_calculator_gpu_deps",
        "//mediapipe/calculators/util:thresholding_calculator",
        "//mediapipe/calculators/util:landmark_projection_calculator",
        "//mediapipe/calculators/tensor:tensors_to_landmarks_calculator",
        "//mediapipe/calculators/tensor:tensors_to_floats_calculator",
        "//mediapipe/calculators/tensor:inference_calculator_interface",
        "//mediapipe/calculators/tensor:inference_calculator_cpu",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:gpu_buffer",
        "//mediapipe/gpu:gpu_shared_data_internal",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "//mediapipe/calculators/core:immediate_mux_calculator",
        "//mediapipe/calculators/core:packet_inner_join_calculator",
        "//mediapipe/calculators/core:previous_loopback_calculator",
        "//mediapipe/calculators/core:merge_calculator",
        "//mediapipe/calculators/core:end_loop_calculator",                     
        "//mediapipe/calculators/core:begin_loop_calculator",          
        "//mediapipe/calculators/util:collection_has_min_size_calculator",
        "//mediapipe/calculators/util:detections_to_render_data_calculator",
        "//mediapipe/calculators/util:labels_to_render_data_calculator",       
        "//mediapipe/calculators/util:landmarks_to_render_data_calculator", 
        "//mediapipe/calculators/util:rect_to_render_data_calculator",
        "//mediapipe/calculators/util:annotation_overlay_calculator_cc_proto",
        "//mediapipe/framework:calculator_options_cc_proto",
        "//mediapipe/util:color_cc_proto",
        "//mediapipe/framework/formats:video_stream_header",
        "//mediapipe/framework/port:vector",
        "//mediapipe/util:render_data_cc_proto",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//mediapipe/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//mediapipe/calculators/tflite:tflite_inference_calculator",
        "//mediapipe/calculators/tensor:inference_calculator",                  # from hand_landmark_cpu dep # empty select applied
#        "//mediapipe/calculators/tensor:tensors_to_classification_calculator",
#        "//mediapipe/calculators/tensor:image_to_tensor_calculator",           # from hand_landmark_cpu dep # cv err due to dep "image_to_tensor_converter_opencv"
#        "//mediapipe/modules/hand_landmark:hand_landmark_cpu",
#        "//mediapipe/util:annotation_renderer", # cv
#        "//mediapipe/calculators/util:annotation_overlay_calculator",
#        "//mediapipe/graphs/hand_tracking/subgraphs:hand_renderer_cpu",
#        "//mediapipe/modules/hand_landmark:hand_landmark_tracking_cpu",
#        "//mediapipe/graphs/hand_tracking:desktop_offline_calculators",
#        "//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
#        "//mediapipe/calculators/tensor:inference_calculator",                  # opengl err
#        "//mediapipe/framework/formats:image_frame_opencv",                    # opencv errs
##        "//mediapipe/framework/port:opencv_core",                             # opencv errs
#        "//mediapipe/framework/formats:image_opencv",                          # opencv errs
#        "//mediapipe/framework/port:opencv_imgproc",                           # opencv errs
#        "//mediapipe/calculators/tensor:image_to_tensor_converter_opencv",     # opencv errs
#        "//mediapipe/framework/formats:image_frame_opencv",                    # opencv errs
#        "//mediapipe/framework/port:opencv_highgui",                           # opencv errs
#        "//mediapipe/framework/port:opencv_video",                             # opencv errs
#        "//mediapipe/calculators/video:opencv_video_decoder_calculator",       # opencv errs
#        "//mediapipe/calculators/video:opencv_video_encoder_calculator",       # opencv errs
#        "//mediapipe/calculators/tensor:tensors_to_classification_calculator",
#        "//mediapipe/calculators/util:landmark_letterbox_removal_calculator",
#        "//mediapipe/calculators/tensor:image_to_tensor_calculator",
#        "//mediapipe/modules/hand_landmark:hand_landmark_gpu",
#       "//mediapipe/modules/hand_landmark:hand_landmark_tracking_gpu",
#        "//mediapipe/graphs/hand_tracking/subgraphs:hand_renderer_gpu",
#        "//mediapipe/modules/hand_landmark:hand_landmark_tracking_gpu",
#    visibility = ["//visibility:public"],
    ],
)

# wasm_cc_binary(
#    name = "hello-world-wasm.js",
#    cc_target = ":hello-world",
#)


