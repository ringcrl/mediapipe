

#include <emscripten/bind.h>

#include "hello-world/cpp/greet.hpp"

using namespace emscripten;
using namespace HelloWorld;

// EMSCRIPTEN_BINDINGS(Hello_World) {
//     emscripten::class_<Greet>("Greet")
//         .constructor<>()
//         .class_function("SayHello", &Greet::SayHello);
      
// }