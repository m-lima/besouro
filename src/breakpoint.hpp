#pragma once

#include <cstdint>
#include <wait.h>

class Breakpoint {
private:
  pid_t mPid;
  std::intptr_t mAddress;
  bool mEnabled;
  std::uint8_t mSavedData;

public:
  Breakpoint(pid_t pid, std::intptr_t address) : mPid{pid}, mAddress{address}, mEnabled{false}, mSavedData{} {}

  void enable();
  void disable();

  auto isEnabled() const -> bool {return mEnabled; }
  auto getAddress() const -> std::intptr_t { return mAddress; }

};
