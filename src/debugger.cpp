#include "debugger.hpp"

#include <wait.h>
#include <sys/ptrace.h>

#include <linenoise/linenoise.hpp>
#include <mfl/string.hpp>
#include <mfl/out.hpp>

namespace {
  struct Command {
    const char * command;
    const char * shortcut;
    const unsigned int expectedArgumentCount;
    void (*executeCommand)(Debugger * instance, const std::vector<std::string> & cmd);

    auto matches(const std::string & str) const -> bool { return str == command || str == shortcut; }
    friend auto operator==(const Command & c1, const Command & c2) -> bool { return c1.command == c2.command; }

    void execute(Debugger * instance, const std::vector<std::string> & cmd) const {
      if (cmd.size() != expectedArgumentCount + 1) {
        mfl::out::println(stderr, "Wrong number of arguments for {:s}. Expected {:d}", command, expectedArgumentCount);
      } else {
        executeCommand(instance, cmd);
      }
    }
  };

  constexpr std::array<Command, 3> COMMANDS = {
      Command{"quit", "q", 0, [](Debugger *, const std::vector<std::string> &) -> void { std::exit(0); }},

      Command{"continue", "c", 0, [](Debugger * instance, const std::vector<std::string> &) -> void {
        instance->continueExecution();
      }},

      Command{"breakpoint", "b", 1, [](Debugger * instance, const std::vector<std::string> & cmd) -> void {
        instance->setBreakpoint(std::stol(cmd[1]));
      }}
  };
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
  auto commandLine = mfl::string::split(line, ' ');

  for (const auto & command : COMMANDS) {
    if (command.matches(commandLine[0])) {
      command.execute(this, commandLine);
      return;
    }
  }

  mfl::out::println(stderr, "Unknown command {:s}", commandLine[0]);
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

void Debugger::setBreakpoint(std::intptr_t address) {
  mfl::out::println("Setting breakpoint at address 0x{:x}", address);

  Breakpoint breakpoint{mPid, address};
  breakpoint.enable();
  mBreakpoints.insert(std::make_pair<std::intptr_t, Breakpoint>(std::move(address), std::move(breakpoint)));
}
