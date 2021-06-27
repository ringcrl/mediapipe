

#include "hello-world/cpp/greet.hpp"
// #include "mediapipe/framework/port.h"



// #include "mediapipe/framework/calculator_graph.h"
// #include "mediapipe/framework/port/logging.h"
// #include "mediapipe/framework/port/parse_text_proto.h"
// #include "mediapipe/framework/port/status.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

// using namespace emscripten;
using namespace HelloWorld;



// #define XSTR(x) STR(x)
// #define STR(x) #x

// #pragma message("The value of MEDIAPIPE_OPENGL_ES_VERSION: " XSTR(MEDIAPIPE_OPENGL_ES_VERSION) ", MEDIAPIPE_OPENGL_ES_31: " XSTR(MEDIAPIPE_OPENGL_ES_31))

// void func1(); 
// void func2(); 
    
// void __attribute__((constructor)) func1(); 
// void __attribute__((destructor)) func2(); 
    
// void func1() 
// { 
//     float f = MEDIAPIPE_OPENGL_ES_VERSION;
//     printf("The value of MEDIAPIPE_OPENGL_ES_VERSION: %f\n", f); 
// } 
    
// void func2() 
// { 
//     float f = MEDIAPIPE_OPENGL_ES_31;
//     printf("The value of MEDIAPIPE_OPENGL_ES_31: %f\n", f); 
// } 



// namespace mediapipe {

// absl::Status PrintHelloWorld() {
//   // Configures a simple graph, which concatenates 2 PassThroughCalculators.
//   CalculatorGraphConfig config =
//       ParseTextProtoOrDie<CalculatorGraphConfig>(R"pb(
//         input_stream: "in"
//         output_stream: "out"
//         node {
//           calculator: "PassThroughCalculator"
//           input_stream: "in"
//           output_stream: "out1"
//         }
//         node {
//           calculator: "PassThroughCalculator"
//           input_stream: "out1"
//           output_stream: "out"
//         }
//       )pb");

//   CalculatorGraph graph;
//   MP_RETURN_IF_ERROR(graph.Initialize(config));
//   ASSIGN_OR_RETURN(OutputStreamPoller poller,
//                    graph.AddOutputStreamPoller("out"));
//   MP_RETURN_IF_ERROR(graph.StartRun({}));
//   // Give 10 input packets that contains the same std::string "Hello World!".
//   for (int i = 0; i < 10; ++i) {
//     MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
//         "in", MakePacket<std::string>("Hello World!").At(Timestamp(i))));
//   }
//   // Close the input stream "in".
//   MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));
//   mediapipe::Packet packet;
//   // Get the output packets std::string.
//   while (poller.Next(&packet)) {
//     LOG(INFO) << packet.Get<std::string>();
//   }
//   return graph.WaitUntilDone();
// }


// }  // namespace mediapipe

std::string helloName(std::string name) {
  return "hello " + name;
}

// int runHello() {
//   google::InitGoogleLogging("hello_world_wasmprac");
//   return mediapipe::PrintHelloWorld().ok();
// }


// int main(int argc, char** argv) {
//   google::InitGoogleLogging(argv[0]);
//   CHECK(mediapipe::PrintHelloWorld().ok());
//   return 0;
// }






// #ifdef __EMSCRIPTEN__

// EMSCRIPTEN_BINDINGS(Hello_World) {
//     emscripten::class_<Greet>("Greet")
//         .constructor<>()
//         .class_function("SayHello", &Greet::SayHello);
      
// }

EMSCRIPTEN_BINDINGS(Hello_World) {
  // function("PrintHelloWorld", &mediapipe::PrintHelloWorld);
  // function("runHello", &runHello);
  emscripten::function("helloName", &helloName);
}

// #endif