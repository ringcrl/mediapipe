#include <emscripten/bind.h>
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/calculator_graph.h"

using namespace emscripten;

// namespace mediapipe {
std::string helloName(std::string name) {
  return "hello " + name;
}

absl::Status PrintHelloWorld() {
  mediapipe::CalculatorGraphConfig config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(R"pb(
      input_stream: "in"
      output_stream: "out"
      node {
        calculator: "PassThroughCalculator"
        input_stream: "in"
        output_stream: "out1"
      }
      node {
        calculator: "PassThroughCalculator"
        input_stream: "out1"
        output_stream: "out"
      }
    )pb");

  mediapipe::CalculatorGraph graph;
  MP_RETURN_IF_ERROR(graph.Initialize(config));
  // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
  //                  graph.AddOutputStreamPoller("out"));

  std::vector<mediapipe::Packet> output_packets;
  
  graph.ObserveOutputStream("out", [&output_packets](const mediapipe::Packet& p) {
    output_packets.push_back(p);
    LOG(INFO) << "inside lambda func: packet.Get<std::string>():" << p.Get<std::string>();
    return absl::OkStatus();
  });

  MP_RETURN_IF_ERROR(graph.StartRun({}));

  absl::SleepFor(absl::Milliseconds(2000));

  
  
  for (int i = 0; i < 5; ++i) {
    mediapipe::Packet p = mediapipe::MakePacket<std::string>("Wasmup!").At(mediapipe::Timestamp(i));
    
    MP_RETURN_IF_ERROR(graph.AddPacketToInputStream("in", p));

    output_packets.emplace_back(p);
  }


  // Close the input stream "in".
  MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));


  // MP_RETURN_IF_ERROR(graph.WaitUntilDone());
  MP_RETURN_IF_ERROR(graph.WaitUntilIdle());


  for (const mediapipe::Packet & p: output_packets) {
    LOG(INFO) << p.Get<std::string>();
  }

  // // Get the output packets std::string.
  // while (poller.Next(&packet)) {
  // }

/*
If we want to be able to repeat it and potentially run multiple frames, then we can just get rid of the "CloseInputStream" call entirely, and can use "graph.WaitUntilIdle()" instead of "graph.WaitUntilDone"
it's fine to just leave input streams open forever, and leave the graph kinda "perpetually running"...  when we're all done with it, we can then just call graph.Close() and it should close and clean up everything at once, so it's a lot simpler/easier that way
*/

  MP_RETURN_IF_ERROR(graph.CloseAllPacketSources());

  MP_RETURN_IF_ERROR(absl::InvalidArgumentError("testing MP_RETURN_IF_ERROR macro"));
  // return absl::OkStatus();
}


std::string runMPGraph() {
  absl::Status status = PrintHelloWorld();

  if (!status.ok()) {
    LOG(WARNING) << "Unexpected error " << status;
  }

  return status.ToString();
}



// } // namespace mediapipe


int main() {}

EMSCRIPTEN_BINDINGS(Hello_World_Simple) {
  // function("PrintHelloWorld", &mediapipe::PrintHelloWorld);
  function("helloName", &helloName);
//   function("helloName", &helloName);
  function("runMPGraph", &runMPGraph);
  
}