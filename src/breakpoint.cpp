#include "breakpoint.hpp"

#include <sys/ptrace.h>

void Breakpoint::enable() {
  auto data = ptrace(PTRACE_PEEKDATA, mPid, mAddress, nullptr);
  mSavedData = static_cast<std::uint8_t >(data & 0xFF);

  auto dataInterrupted = ((data & ~0xFF) | 0xCC);
  ptrace(PTRACE_POKEDATA, mPid, mAddress, dataInterrupted);

  mEnabled = true;
}

void Breakpoint::disable() {
  auto data = ptrace(PTRACE_PEEKDATA, mPid, mAddress, nullptr);
  auto dataRestored = ((data & ~0xFF) | mSavedData);
  ptrace(PTRACE_POKEDATA, mPid, mAddress, dataRestored);

  mEnabled = false;
}
