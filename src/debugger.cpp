#include "debugger.hpp"

#include <wait.h>
#include <sys/ptrace.h>
#include <iterator>

#include <linenoise/linenoise.hpp>
#include <mfl/string.hpp>
#include <mfl/out.hpp>
#include <sys/user.h>

namespace {
  struct Command {
    const char * command;
    const char * shortcut;
    const unsigned int expectedArgumentCount;
    void (* executeCommand)(Debugger * instance, const std::vector<std::string> & cmd);

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
}

struct Debugger::Commands {
  constexpr static std::array<Command, 3> AVAILABLE {
      {
          {"quit", "q", 0, [](Debugger *, const std::vector<std::string> &) -> void { std::exit(0); }},

          {"continue", "c", 0, [](Debugger * instance, const std::vector<std::string> &) -> void {
            instance->continueExecution();
          }},

          {"breakpoint", "b", 1, [](Debugger * instance, const std::vector<std::string> & cmd) -> void {
            instance->setBreakpoint(std::stol(cmd[1]));
          }}
      }
  };
};

void Debugger::launch(char * prog) {
  if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0) {
    mfl::out::println(stderr, "Error with PTRACE");
  } else {
    execl(prog, nullptr);
  }
}

void Debugger::run() {
  waitForSignal();

  std::string line;
  while (!linenoise::Readline("besouro> ", line)) {
    if (line.empty()) {
      continue;
    }

    linenoise::AddHistory(line.c_str());
    handleCommand(line);
  }
}

void Debugger::handleCommand(const std::string & line) {
  auto commandLine = mfl::string::split(line, ' ');

  for (const auto & command : Commands::AVAILABLE) {
    if (command.matches(commandLine[0])) {
      command.execute(this, commandLine);
      return;
    }
  }

  mfl::out::println(stderr, "Unknown command {:s}", commandLine[0]);
}

void Debugger::continueExecution() {
  stepOverBreakpoint();
  ptrace(PTRACE_CONT, mPid, nullptr, nullptr);
  waitForSignal();
}

void Debugger::setBreakpoint(std::intptr_t address) {
  mfl::out::println("Setting breakpoint at address 0x{:x}", address);

  Breakpoint breakpoint{mPid, address};
  breakpoint.enable();
  mBreakpoints[address] = breakpoint;
}

std::uint64_t Debugger::getRegister(Register::Register reg) {
  user_regs_struct registers;
  ptrace(PTRACE_GETREGS, mPid, nullptr, &registers);

  auto it = std::find_if(cbegin(Register::DESCRIPTORS),
                         cend(Register::DESCRIPTORS),
                         [reg](auto && descriptor) { return descriptor.reg == reg; });

  return *(reinterpret_cast<std::uint64_t*>(&registers) + (it - std::begin(Register::DESCRIPTORS)));
}

void Debugger::setRegister(Register::Register reg, std::uint64_t value) {
  user_regs_struct registers;
  ptrace(PTRACE_GETREGS, mPid, nullptr, &registers);

  auto it = std::find_if(cbegin(Register::DESCRIPTORS),
                         cend(Register::DESCRIPTORS),
                         [reg](auto && descriptor) { return descriptor.reg == reg; });

  *(reinterpret_cast<std::uint64_t*>(&registers) + (it - std::begin(Register::DESCRIPTORS))) = value;
}

void Debugger::waitForSignal() {
  int status;
  waitpid(mPid, &status, 0);
}

void Debugger::stepOverBreakpoint() {
  auto possibleBreakpointLocation = getPC() - 1;

  if (mBreakpoints.count(possibleBreakpointLocation)) {
    auto & breakpoint = mBreakpoints[possibleBreakpointLocation];

    if (breakpoint.isEnabled()) {
      auto previousInstructionAddress = possibleBreakpointLocation;
      setPC(previousInstructionAddress);

      breakpoint.disable();
      ptrace(PTRACE_SINGLESTEP, mPid, nullptr, nullptr);
      waitForSignal();
      breakpoint.enable();
    }
  }
}

std::optional<dwarf::die> Debugger::getFunctionFromPC(std::uint64_t pc) {
  for (const auto & cu : mDwarf.compilation_units()) {
    if (dwarf::die_pc_range(cu.root()).contains(pc)) {
      for (const auto & die : cu.root()) {
        if (die.tag == dwarf::DW_TAG::subprogram) {
          if (dwarf::die_pc_range(die).contains(pc)) {
            return std::make_optional(die);
          }
        }
      }
    }
  }

  mfl::out::println(stderr, "Could not find function at 0x{:x}", pc);
  return {};
}

std::optional<dwarf::line_table::iterator> Debugger::getLineFromPC(std::uint64_t pc) {
  for (const auto & cu : mDwarf.compilation_units()) {
    if (dwarf::die_pc_range(cu.root()).contains(pc)) {
      auto & lineTable = cu.get_line_table();
      auto it = lineTable.find_address(pc);
      if (it == lineTable.end()) {
        mfl::out::println(stderr, "Could not find line at 0x{:x}", pc);
        return {};
      } else {
        return std::make_optional(it);
      }
    }
  }

  mfl::out::println(stderr, "Could not find line at 0x{:x}", pc);
  return {};
}

void Debugger::printSource(const std::string & fileName, unsigned int line, unsigned int lineContext) {
  std::ifstream file{fileName};

  auto startLine = line <= lineContext ? 1 : line - lineContext;
  auto endLine = line + lineContext + (line < lineContext ? lineContext - line : 0);

  char c{};

  auto currentLine = 1u;
}
