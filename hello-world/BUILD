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

cc_binary(
    name = "hello-world-wasm.js",
    srcs = ["main.cpp"],
#    linkopts = DEFAULT_EMSCRIPTEN_LINKOPTS + WASM_LINKOPTS,
    deps = [
        "//hello-world/cpp:greet",
#       "//mediapipe/calculators/core:constant_side_packet_calculator",
#       "//mediapipe/calculators/core:flow_limiter_calculator",
#       "//mediapipe/graphs/hand_tracking/subgraphs:hand_renderer_gpu",
        "//mediapipe/calculators/core:gate_calculator_cc_proto",
        "//mediapipe/framework:counter",
        "//mediapipe/framework:port",
        "//mediapipe/framework/port:integral_types",
        "@com_google_absl//absl/time",
#        "//third_party:glog",
#        "//mediapipe/framework/port:logging",
#        "//mediapipe/framework/deps:map_util",
#        "//mediapipe/framework/port:map_util"
#        "//mediapipe/framework/port:map_util"
#        "//mediapipe/framework:counter_factory",
#        "//mediapipe/framework:calculator_state",
#        "//mediapipe/framework:calculator_context",
#        "//mediapipe/framework:calculator_base",
#        "//mediapipe/framework:calculator_framework",
#       "//mediapipe/calculators/core:gate_calculator",
#       "//mediapipe/modules/hand_landmark:hand_landmark_gpu",
#       "//mediapipe/modules/hand_landmark:hand_landmark_tracking_gpu",
#"//visibility:public"
    ],
)

# wasm_cc_binary(
#    name = "hello-world-wasm.js",
#    cc_target = ":hello-world",
#)