#include <unistd.h>

#include <mfl/out.hpp>

#include "debugger.hpp"

void printUsage(std::FILE * output) {
  mfl::out::println(output, "Usage for besouro:");
  mfl::out::println(output, "besouro PROGRAM [ARGS]");
  mfl::out::println(output, "  {:<15}{}", "PROGRAM", "Which program to debug");
  mfl::out::println(output, "  {:<15}{}", "ARGS", "Arguments to be passed to PROGRAM");
}

int main(int argc, char * argv[]) {
  if (argc < 2) {
    printUsage(stderr);
    return -1;
  }

  auto prog = argv[1];

  auto pid = fork();
  if (pid == 0) {
    Debugger::launch(prog);
  } else if (pid >= 1) {
    Debugger debugger{prog, pid};
    debugger.run();
  }
}
