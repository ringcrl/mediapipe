#include <emscripten/bind.h>
using namespace emscripten;


std::string helloName(std::string name) {
  return "hello " + name;
}

int main() {}

EMSCRIPTEN_BINDINGS(Hello_World_Simple) {
  // function("PrintHelloWorld", &mediapipe::PrintHelloWorld);
  function("helloName", &helloName);
//   function("helloName", &mediapipe::helloName);
}