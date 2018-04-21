#pragma once

#include <string>
#include <unordered_map>

#include "breakpoint.hpp"

class Debugger {
private:
  std::string mProgramName;
  pid_t mPid;
  std::unordered_map<std::intptr_t, Breakpoint> mBreakpoints;

public:
  Debugger(std::string programName, pid_t pid) : mProgramName{std::move(programName)}, mPid{pid} {}

  void run();
  void handleCommand(const std::string & line);
  void continueExecution();
  void setBreakpoint(std::intptr_t address);

  static void launch(char * prog);
};