#!/bin/bash

set -eof

bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
  mediapipe/examples/desktop/selfie_segmentation:selfie_segmentation_gpu

GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/selfie_segmentation/selfie_segmentation_gpu \
  --calculator_graph_config_file=mediapipe/graphs/selfie_segmentation/selfie_segmentation_gpu.pbtxt
  