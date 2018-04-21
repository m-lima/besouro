#pragma once

#include <string>

class Debugger {
private:
  std::string mProgramName;
  pid_t mPid;

public:
  Debugger(std::string programName, pid_t pid) : mProgramName{std::move(programName)}, mPid{pid} {}

  void run();
  void handleCommand(const std::string & line);
  void continueExecution();

  static void launch(char * prog);
};