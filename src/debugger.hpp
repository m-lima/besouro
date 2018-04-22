#pragma once

#include <string>
#include <unordered_map>
#include <fcntl.h>

#include <elf/elf++.hh>
#include <dwarf/dwarf++.hh>

#include "breakpoint.hpp"
#include "registers.hpp"

class Debugger {
private:
  struct Commands;

  std::string mProgramName;
  pid_t mPid;
  std::unordered_map<std::intptr_t, Breakpoint> mBreakpoints;
  elf::elf mElf;
  dwarf::dwarf mDwarf;

  void handleCommand(const std::string & line);

  void continueExecution();
  void setBreakpoint(std::intptr_t address);
  std::uint64_t getRegister(Register::Register reg);
  void setRegister(Register::Register reg, std::uint64_t value);
  void waitForSignal();
  void stepOverBreakpoint();
  std::optional<dwarf::die> getFunctionFromPC(std::uint64_t pc);
  std::optional<dwarf::line_table::iterator> getLineFromPC(std::uint64_t pc);
  void printSource(const std::string & fileName, unsigned int line, unsigned int lineContext);

  auto getPC() -> std::uint64_t { return getRegister(Register::RIP); }
  auto setPC(std::uint64_t pc) -> void { setRegister(Register::RIP, pc); }

public:
  Debugger(std::string programName, pid_t pid)
      : mProgramName{std::move(programName)},
        mPid{pid} {
    auto file = open(mProgramName.c_str(), O_RDONLY);

    mElf = elf::elf{elf::create_mmap_loader(file)};
    mDwarf = dwarf::dwarf{dwarf::elf::create_loader(mElf)};
  }

  void run();

  static void launch(char * prog);
};