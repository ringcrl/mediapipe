load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")

package(default_visibility = ["//visibility:public"])

DEFAULT_EMSCRIPTEN_LINKOPTS = [
    #    "-flto",                            # Specify lto (has to be set on for compiler as well), to disable use: -fno-lto
    "--bind",  # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "--closure 1",  # Run the closure compiler
    "-s MALLOC=emmalloc",  # Switch to using the much smaller implementation
    "-s ALLOW_MEMORY_GROWTH=0",  # Our example doesn't need memory growth
    "-s USE_PTHREADS=1",  # Disable pthreads
    "-s ASSERTIONS=0",  # Turn off assertions
    "-s EXPORT_ES6=1",  # Export as es6 module, used for rollup
    "-s MODULARIZE=1",  # Allows us to manually invoke the initializatio of wasm
    "-s EXPORT_NAME=createModule",  # Not used, but good to specify
    "-s USE_ES6_IMPORT_META=0",  # Disable loading from import meta since we use rollup
    "-s SINGLE_FILE=1",  # Pack all webassembly into base64
    "-s DISABLE_EXCEPTION_CATCHING=1",  # Disable all exception catching
    "-s NODEJS_CATCH_EXIT=0",  # We don't have a 'main' so disable exit() catching
]

WASM_LINKOPTS = [
    "-s WASM=1",  # Specify wasm output
]

# notes
# warning: Link with `-s LLD_REPORT_UNDEFINED` to get more information on undefined symbols


cc_library(
    name = "canvas_renderer",
    hdrs = ["renderer.h"],
    srcs = ["renderer.cc"],
    deps = [
        "//mediapipe/framework/port:logging",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gpu_buffer",
        "//mediapipe/gpu:gpu_shared_data_internal",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:image_frame_to_gpu_buffer_calculator",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/gpu:gl_simple_calculator",
        "//mediapipe/gpu:gl_simple_shaders",
    ]
)


cc_binary(
    name = "hello-world-simple",
    srcs = [
        "main2.cpp",
        # "renderer.h", 
        # "renderer.cc"
    ],
    
    linkopts = [
        "-s USE_PTHREADS=0",
        "-s ALLOW_MEMORY_GROWTH=1",
        "-s ERROR_ON_UNDEFINED_SYMBOLS=1",
        "-s USE_WEBGL2=1",
        "--export-all",
        # "--no-entry",
        "-s ASSERTIONS=1",
        "--bind",
        "-s STANDALONE_WASM=0", # generates only wasm file, not .js file? generates independent wasm, but missing utilities of js
        "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/assets@assets",
        "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/mediapipe/modules/face_detection/face_detection_short_range.tflite@mediapipe/modules/face_detection/face_detection_short_range.tflite",
        # "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/mediapipe/@mediapipe/",
        "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/mediapipe/modules/selfie_segmentation/selfie_segmentation.tflite@mediapipe/modules/selfie_segmentation/selfie_segmentation.tflite",
        "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/mediapipe/modules/selfie_segmentation/selfie_segmentation_landscape.tflite@mediapipe/modules/selfie_segmentation/selfie_segmentation_landscape.tflite",
        "--preload-file /home/pinku/work/src/github.com/prantoran/mediapipe/mediapipe/modules/face_landmark/face_landmark.tflite@mediapipe/modules/face_landmark/face_landmark.tflite",

    ],
    visibility = ["//visibility:public"],
    deps = [
        # ":canvas_renderer",
        "//hello-world/cpp:popen",
        "//mediapipe/calculators/core:pass_through_calculator",
        "//mediapipe/calculators/image:recolor_calculator",
        "//mediapipe/framework:calculator_graph",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:logging",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/gpu:gpu_buffer",
        "//mediapipe/gpu:gpu_shared_data_internal",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:image_frame_to_gpu_buffer_calculator",
        "//mediapipe/gpu:gl_simple_calculator",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/modules/face_detection:face_detection_short_range_gpu",
        "//mediapipe/modules/selfie_segmentation:selfie_segmentation_gpu",
        "//mediapipe/graphs/selfie_segmentation:selfie_segmentation_gpu_deps",
        # face mesh
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/graphs/face_mesh/subgraphs:face_renderer_gpu",
        "//mediapipe/modules/face_landmark:face_landmark_front_gpu",
        "//mediapipe/framework/formats:landmark_cc_proto",
    ],
)

# generates only wasm file, not .js file? generates independent wasm, but missing utilities of js
cc_binary(
    name = "hello-world-wasm",
    srcs = ["main.cpp"],
    linkopts = [
        "-s USE_PTHREADS=0",
        "-s ALLOW_MEMORY_GROWTH=0",
        "-s ERROR_ON_UNDEFINED_SYMBOLS=1", 
        "-s USE_WEBGL2=1",
        "--export-all",
        "--no-entry",
        "-s ASSERTIONS=1",
        "--fatal-warnings",
        "--bind",
        # "-s STANDALONE_WASM=1",
    ],
    visibility = ["//visibility:public"],
    #    linkopts = DEFAULT_EMSCRIPTEN_LINKOPTS + WASM_LINKOPTS,
    deps = [
        # "//mediapipe/graphs/iris_tracking/subgraphs:renderer_calculators",
        "//mediapipe/graphs/iris_tracking/calculators:iris_to_depth_calculator",
        # "//mediapipe/graphs/iris_tracking/subgraphs:iris_and_depth_renderer_gpu",
        "//mediapipe/graphs/iris_tracking/calculators:update_face_landmarks_calculator",
        # "//mediapipe/graphs/iris_tracking:iris_tracking_gpu_deps",
        # -------------- mp/examples/desktop/iris_tracking:iris_tracking_gpu end ---------------
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "//mediapipe/graphs/face_mesh/subgraphs:renderer_calculators",
        "//mediapipe/graphs/face_mesh/subgraphs:face_renderer_gpu",
        "//mediapipe/graphs/face_mesh:desktop_live_gpu_calculators",
        # -------------- mp/examples/desktop/face_mesh:face_mesh_gpu end ---------------
        "//mediapipe/graphs/face_detection:desktop_live_gpu_calculators",
        "//mediapipe/graphs/face_detection:desktop_live_calculators",
        # -------------- mp/examples/desktop/face_detection:face_detection_gpu end ---------------
        # "//mediapipe/framework:calculator_framework", # included before
        # "//mediapipe/framework/port:file_helpers", # ,,
        # "//mediapipe/framework/port:map_util", # ,,
        # "//mediapipe/framework/port:parse_text_proto", # ,,
        # "//mediapipe/framework/port:ret_check", # ,,
        # "//mediapipe/framework/port:status", # ,,
        # "//mediapipe/framework/port:statusor", # ,,
        # "@com_google_absl//absl/flags:flag", # ,,
        # "@com_google_absl//absl/flags:parse", # ,,
        # "@com_google_absl//absl/strings", # ,,
        # -------------- mp/examples/desktop:simple_run_graph_main end ---------------
        "//mediapipe/calculators/core:pass_through_calculator",
        # "//mediapipe/framework:calculator_graph", # included before
        # "//mediapipe/framework/port:logging",
        # "//mediapipe/framework/port:parse_text_proto",
        # "//mediapipe/framework/port:status",
        # -------------- desktop helloworld end --------------------
        # "@maven//:com_google_guava_guava", # no matching toolchains found for types @bazel_tools//tools/android:sdk_toolchain_type
        # "@maven//:androidx_concurrent_concurrent_futures", # ,,
        # "//third_party:androidx_constraint_layout", # ,,
        # "//third_party:androidx_appcompat", # ,,
        # "//mediapipe/java/com/google/mediapipe/glutil", # ,,
        # "//mediapipe/java/com/google/mediapipe/framework:android_framework", # ,,
        # "//mediapipe/java/com/google/mediapipe/components:android_components", # ,,
        # "//mediapipe/java/com/google/mediapipe/components:android_camerax_helper", # ,, 
        # -------------- Android deps end --------------------
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
        "//mediapipe/framework:stream_handler_cc_proto",  # solved by adding hello-world to mediapipe_internal
        "//mediapipe/framework:mediapipe_options_cc_proto",  # solved by adding hello-world to mediapipe_internal
        "//mediapipe/framework/stream_handler:default_input_stream_handler",  # popen
        "//mediapipe/framework/stream_handler:in_order_output_stream_handler",  # popen
        "//mediapipe/framework:calculator_node",  # popen recursed from here
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
        # "//mediapipe/calculators/tflite:tflite_inference_calculator", # deprecated, equivalent in tensor is latest
        "//mediapipe/calculators/tensor:inference_calculator",  # from hand_landmark_cpu dep # empty select applied to exclude "inference_calculator_gl"
        "//mediapipe/calculators/tensor:tensors_to_classification_calculator_cc_proto",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "//mediapipe/framework/formats:classification_cc_proto",
        "//mediapipe/util:resource_util",
        "//mediapipe/calculators/tensor:tensors_to_classification_calculator", # android file err, added select emscripten_wasm for android file header dep
        "//mediapipe/calculators/tensor:image_to_tensor_converter_gl_texture",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/calculators/util:landmark_letterbox_removal_calculator",
        "//mediapipe/calculators/util:landmark_projection_calculator_cc_proto",
        "//mediapipe/calculators/util:thresholding_calculator_cc_proto",
        "@com_google_protobuf//:protobuf",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats/annotation:locus_cc_proto",
        "//mediapipe/framework:type_map",
        "//mediapipe/framework/port:point",
        "//mediapipe/framework/port:rectangle",
        "//mediapipe/framework/formats/annotation:rasterization_cc_proto",
        "//mediapipe/framework/formats:location",  # used by "calculator/tensor:tensors_to_classification_calculator", "//mediapipe/calculators/util:landmark_letterbox_removal_calculator", cv err, location.h uses cv::Rect, cv::Mat,
        "//mediapipe/gpu:gl_context",
        "//mediapipe/calculators/tensor:image_to_tensor_calculator",  # from hand_landmark_cpu dep # cv err due to dep "image_to_tensor_converter_opencv"
        "//mediapipe/modules/hand_landmark:hand_landmark_cpu",
        # "//mediapipe/util:annotation_renderer",  # uses cv:: many times, required by "//mediapipe/calculators/util:annotation_overlay_calculator"
        # "//mediapipe/calculators/util:annotation_overlay_calculator", #uses cv
        "//mediapipe/graphs/hand_tracking/subgraphs:hand_renderer_cpu", # used select to exclude "//mediapipe/calculators/util:annotation_overlay_calculator" 
        "//mediapipe/modules/hand_landmark:hand_landmark_tracking_cpu",
        "@com_google_googletest//:gtest",
        "//mediapipe/graphs/hand_tracking/subgraphs:hand_renderer_gpu", # 1 dep uses cv ("//mediapipe/calculators/util:annotation_overlay_calculator"), excluded using select
        "//mediapipe/modules/hand_landmark:hand_landmark_tracking_gpu",
        "//mediapipe/calculators/image:image_properties_calculator",
        "//mediapipe/calculators/util:association_norm_rect_calculator",
        "//mediapipe/calculators/util:filter_collection_calculator",
        "//mediapipe/modules/palm_detection:palm_detection_gpu",
        "//mediapipe/graphs/hand_tracking:mobile_calculators",
        # "//third_party:opencv",
        # "//mediapipe/framework/formats:image_frame_opencv",  # opencv errs
        # "//mediapipe/framework/formats:image_opencv",  # opencv errs
        # "//mediapipe/framework/port:opencv_core",  # opencv errs
        # "//mediapipe/framework/port:opencv_imgproc",  # opencv errs
        # "//mediapipe/framework/port:opencv_highgui",  # opencv errs
        # "//mediapipe/framework/port:opencv_video",  # opencv errs
        # "//mediapipe/calculators/tensor:image_to_tensor_converter_opencv",  # opencv errs
        # "//mediapipe/calculators/video:opencv_video_decoder_calculator",  # opencv errs
        # "//mediapipe/calculators/video:opencv_video_encoder_calculator",  # opencv errs
        # "//mediapipe/examples/desktop:demo_run_graph_main_gpu",
        # "//mediapipe/framework/port:status_matchers",
        # "//mediapipe/framework/deps:status_matchers",
        # "//mediapipe/framework/port:gtest_main",
        # "//mediapipe/graphs/hand_tracking:desktop_offline_calculators",
        # "//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        # "//mediapipe/modules/hand_landmark:hand_landmark_gpu",
    ],
)

# wasm_cc_binary(
#    name = "hello-world-wasm.js",
#    cc_target = ":hello-world",
#)