#pragma once
// What a recompiler case's checks run on: the generated code, a guest memory holding the case's image, and stand ins
// for the runtime services the generated code calls.
#include <doctest/doctest.h>

#include <cstring>
#include <string>
#include <vector>

#include "case_symbols.h"
#include "x86_recomp_shared.h"

// x86_trap throws this, so a check can expect a trap
struct GuestTrap {
    uint32_t va;
    std::string what;
};

struct Guest {
    static constexpr uint32_t kStackTop = 0x800000;
    static constexpr uint32_t kReturn = 0xFFFFFFF0;   // never a target: a call returns through the host stack
    static constexpr uint32_t kScratch = 0x400000;    // writable, free for a check's own data
    static constexpr uint32_t kMemory = 0x1000000;

    Guest();

    X86Context ctx;
    std::vector<uint8_t> memory;
    uint8_t* base;

    // calls fn as the guest would and checks it popped what it should
    void Call(X86Func* fn);
    // a function the check knows only by address
    void CallAddress(uint32_t va);
    // fn's stack after it returned, without the check
    void CallUnchecked(X86Func* fn);

    template <typename T>
    T Read(uint32_t va) const {
        T v;
        std::memcpy(&v, base + va, sizeof(T));
        return v;
    }
    template <typename T>
    void Write(uint32_t va, T v) {
        std::memcpy(base + va, &v, sizeof(T));
    }
    uint32_t R32(uint32_t va) const { return Read<uint32_t>(va); }
};

// the recompiled function at va, null if the analysis did not make one
X86Func* FindFunction(uint32_t va);
bool IsContinuation(uint32_t va);
// the ordinals x86_kernel saw, oldest first; each reached export returns its ordinal
extern std::vector<uint32_t> g_kernel_calls;
