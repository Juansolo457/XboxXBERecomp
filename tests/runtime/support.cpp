#define DOCTEST_CONFIG_IMPLEMENT
#include "support.h"

#include <chrono>
#include <cstdio>
#include <functional>
#include <thread>

#include "audio/audio.h"
#include "xbe/xbe.h"

// what kernel/boot.cpp holds; that file also needs the app and a game
namespace kernel {
uint8_t* g_base = nullptr;
const xbe::Image* g_image = nullptr;
}  // namespace kernel

namespace audio {
void StartOutput(std::function<void(float*, uint32_t)>) {}
}  // namespace audio

// on Linux cpu/dispatch.cpp expects the recompiled code linked in; these images have none
#ifndef _WIN32
extern "C" const X86FuncMapping X86FuncMappings[] = {{0, nullptr}};
extern "C" const uint32_t X86Continuations[] = {0};
extern "C" const uint32_t X86CodeBase = 0, X86CodeEnd = 0, X86EntryPoint = 0;
#endif

namespace test {

GuestStack::GuestStack() {
    top = mem::AllocStack(0x20000);
    ctx.esp = top - 64;
    ctx.fcw = 0x27F;
    ctx.mxcsr = 0x1F80;
}

GuestStack::~GuestStack() { mem::FreeStack(top); }

uint32_t KernelCall(X86Context& ctx, uint32_t ordinal, std::initializer_list<uint32_t> args) {
    const uint32_t esp = ctx.esp;
    for (auto it = std::rbegin(args); it != std::rend(args); ++it) {
        ctx.esp -= 4;
        kernel::W32(ctx.esp, *it);
    }
    ctx.esp -= 4;
    kernel::W32(ctx.esp, 0xFFFFFFF0u);
    x86_kernel(ctx, kernel::g_base, ordinal);
    REQUIRE(ctx.esp == esp);  // the export popped its arguments and the return address
    return ctx.eax;
}

bool WaitUntilImpl(bool (*poll)(void*), void* arg) {
    auto end = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() < end) {
        if (poll(arg)) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return false;
}

}  // namespace test

int main(int argc, char** argv) {
    if (!mem::Init()) return 1;
    kernel::g_base = mem::Base();
    return doctest::Context(argc, argv).run();
}
