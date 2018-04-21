#include "debugger.hpp"

#include <wait.h>
#include <sys/ptrace.h>

#include <linenoise/linenoise.hpp>
#include <mfl/string.hpp>

namespace {
  namespace Command {
    constexpr auto CONTINUE = "continue";
    constexpr auto C = "c";
  }
}

void Debugger::run() {
  int waitStatus = 0;
  int options = 0;
  waitpid(mPid, &waitStatus, options);

  std::string line;
  while(!linenoise::Readline("besouro> ", line)) {
    if (line.empty()) {
      continue;
    }

    linenoise::AddHistory(line.c_str());
    handleCommand(line);
  }
}

void Debugger::handleCommand(const std::string & line) {
  auto args = mfl::string::split(line, ' ');
  auto command = args[0];

  if (command == Command::CONTINUE || command == Command::C) {
    continueExecution();
  }
}

void Debugger::continueExecution() {
  ptrace(PTRACE_CONT, mPid, nullptr, nullptr);

  int waitStatus;
  int options = 0;
  waitpid(mPid, &waitStatus, options);
}

void Debugger::launch(char * prog) {
  ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
  execl(prog, nullptr);
}
