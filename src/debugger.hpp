#pragma once

#include <string>
#include <unordered_map>

#include "breakpoint.hpp"
#include "registers.hpp"

class Debugger {
private:
  std::string mProgramName;
  pid_t mPid;
  std::unordered_map<std::intptr_t, Breakpoint> mBreakpoints;

  void handleCommand(const std::string & line);
  std::uint64_t getRegister(Register::Register reg);
  void setRegister(Register::Register reg, std::uint64_t value);
  void waitForSignal();
  void stepOverBreakpoint();

  auto getPC() -> std::uint64_t { return getRegister(Register::RIP); }
  auto setPC(std::uint64_t pc) -> void { setRegister(Register::RIP, pc); }

public:
  Debugger(std::string programName, pid_t pid) : mProgramName{std::move(programName)}, mPid{pid} {}

  static void launch(char * prog);

  void continueExecution();
  void setBreakpoint(std::intptr_t address);

  void run();
};