#pragma once

#include <string>
#include <array>

namespace Register {

  enum Register {
    RAX,
    RBX,
    RCX,
    RDX,
    RDI,
    RSI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RIP,
    R_FLAGS,
    CS,
    ORIG_RAX,
    FS_BASE,
    GS_BASE,
    FS,
    GS,
    SS,
    DS,
    ES
  };

  struct Descriptor {
    Register reg;
    int dwarf;
    const char * name;
  };

  constexpr std::array<Descriptor, 27> DESCRIPTORS{
      {
          {Register::R15,      15,      "r15"},
          {Register::R14,      14,      "r14"},
          {Register::R13,      13,      "r13"},
          {Register::R12,      12,      "r12"},
          {Register::RBP,       6,      "rbp"},
          {Register::RBX,       3,      "rbx"},
          {Register::R11,      11,      "r11"},
          {Register::R10,      10,      "r10"},
          {Register::R9,        9,       "r9"},
          {Register::R8,        8,       "r8"},
          {Register::RAX,       0,      "rax"},
          {Register::RCX,       2,      "rcx"},
          {Register::RDX,       1,      "rdx"},
          {Register::RSI,       4,      "rsi"},
          {Register::RDI,       5,      "rdi"},
          {Register::ORIG_RAX, -1, "orig_rax"},
          {Register::RIP,      -1,      "rip"},
          {Register::CS,       51,       "cs"},
          {Register::R_FLAGS,  49,   "eflags"},
          {Register::RSP,       7,      "rsp"},
          {Register::SS,       52,       "ss"},
          {Register::FS_BASE,  58,  "fs_base"},
          {Register::GS_BASE,  59,  "gs_base"},
          {Register::DS,       53,       "ds"},
          {Register::ES,       50,       "es"},
          {Register::FS,       54,       "fs"},
          {Register::GS,       55,       "gs"}
      }
  };

}
