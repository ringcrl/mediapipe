load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "greet",
    srcs = ["greet.cpp"],
    hdrs = ["greet.hpp"]
)


cc_library(
    name = "popen",
    srcs = ["popen.cc"],
    hdrs = ["popen.hpp"],
    deps = [
        "//mediapipe/framework/port:status",
    ]
)