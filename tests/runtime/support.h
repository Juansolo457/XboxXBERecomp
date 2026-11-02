#pragma once
// The kernel and XDK sources as the game links them, on the real guest address space, with no game image.
#include <doctest/doctest.h>

#include <initializer_list>

#include "kernel/kernel.h"

namespace dispatch {
uint32_t HostThunk(X86Func* fn);
}

namespace test {

// a context with its own guest stack, as a guest thread starts
struct GuestStack {
    X86Context ctx{};
    GuestStack();
    ~GuestStack();
    uint32_t top = 0;
};

// calls a kernel export by ordinal the way recompiled code does, returns eax
uint32_t KernelCall(X86Context& ctx, uint32_t ordinal, std::initializer_list<uint32_t> args);

// spins until the condition holds, false after a few seconds
bool WaitUntilImpl(bool (*poll)(void*), void* arg);
template <typename F>
bool WaitUntil(F condition) {
    return WaitUntilImpl([](void* p) { return (*static_cast<F*>(p))(); }, &condition);
}

}  // namespace test
