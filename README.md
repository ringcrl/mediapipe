---
layout: default
title: Home
nav_order: 1
---

<!-- ![GSOC](docs/images/gsoc.png) -->
![MediaPipe](docs/images/mediapipe_small.png) 

# MediaPipe GSOC 2021: Web Video Effects App
- Built on-top of the MediaPipe project.
- The Emscripten mapping and the BUILD files for the project are in `hello-world`.


## How to run the project
- Set up MediaPipe using the [official instructions](https://google.github.io/mediapipe/getting_started/cpp.html).
    - The WebGL rendering via WASM uses GPU, so we need GPU version of MediaPipe setup.
    - The development environment for the project was Linux.
- From the root directory of the project, run the command in the terminal:
    - `make build && make run`
    - The scripts that are executed are in `scripts/...` and `Makefile`.
