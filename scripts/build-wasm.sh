#!/usr/bin/env bash
set -euo pipefail


# Build the WASM variant
bazel build //hello-world:hello-world-wasm --config=wasm
