

#include <emscripten/bind.h>

#include "hello-world/cpp/greet.hpp"
#include "mediapipe/framework/port.h"

using namespace emscripten;
using namespace HelloWorld;



#define XSTR(x) STR(x)
#define STR(x) #x

#pragma message("The value of MEDIAPIPE_OPENGL_ES_VERSION: " XSTR(MEDIAPIPE_OPENGL_ES_VERSION) ", MEDIAPIPE_OPENGL_ES_31: " XSTR(MEDIAPIPE_OPENGL_ES_31))

void func1(); 
void func2(); 
    
void __attribute__((constructor)) func1(); 
void __attribute__((destructor)) func2(); 
    
void func1() 
{ 
    float f = MEDIAPIPE_OPENGL_ES_VERSION;
    printf("The value of MEDIAPIPE_OPENGL_ES_VERSION: %f\n", f); 
} 
    
void func2() 
{ 
    float f = MEDIAPIPE_OPENGL_ES_31;
    printf("The value of MEDIAPIPE_OPENGL_ES_31: %f\n", f); 
} 


#ifdef __EMSCRIPTEN__

// int main() {}
// EMSCRIPTEN_BINDINGS(Hello_World) {
//     emscripten::class_<Greet>("Greet")
//         .constructor<>()
//         .class_function("SayHello", &Greet::SayHello);
      
// }

#endif
