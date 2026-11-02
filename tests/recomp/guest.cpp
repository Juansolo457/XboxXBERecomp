#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "guest.h"

#include <fstream>
#include <iterator>

#include "xbe/xbe.h"

// hle.h reads guest memory through this; Guest points it at its memory
namespace kernel {
extern uint8_t* g_base;
uint8_t* g_base = nullptr;
}  // namespace kernel

std::vector<uint32_t> g_kernel_calls;

X86Func* FindFunction(uint32_t va) {
    for (const X86FuncMapping* m = X86FuncMappings; m->host; m++)
        if (m->guest == va) return m->host;
    return nullptr;
}

bool IsContinuation(uint32_t va) {
    for (const uint32_t* c = X86Continuations; *c; c++)
        if (*c == va) return true;
    return false;
}

Guest::Guest() : ctx{}, memory(kMemory) {
    base = memory.data();
    kernel::g_base = base;
    std::ifstream f(XBE_PATH, std::ios::binary);
    REQUIRE(f);
    std::vector<uint8_t> file((std::istreambuf_iterator<char>(f)), {});
    xbe::Image img;
    std::string err;
    REQUIRE_MESSAGE(img.Parse(std::move(file), err), err);
    for (auto& s : img.sections) std::memcpy(base + s.va, img.file.data() + s.raw, s.rsize);
    ctx.fcw = 0x27F;  // as threads start: 53 bit precision, exceptions masked
    ctx.mxcsr = 0x1F80;
    ctx.fs_base = kScratch + 0x1000;
    g_kernel_calls.clear();
}

void Guest::CallUnchecked(X86Func* fn) {
    ctx.esp = kStackTop - 4;
    Write<uint32_t>(ctx.esp, kReturn);
    fn(ctx, base);
}

void Guest::Call(X86Func* fn) {
    CallUnchecked(fn);
    CHECK(ctx.esp == kStackTop);
}

void Guest::CallAddress(uint32_t va) {
    X86Func* fn = FindFunction(va);
    REQUIRE(fn);
    Call(fn);
}

// the runtime services, as far as these images use them

void x86_icall(X86Context& ctx, uint8_t* base, uint32_t target) {
    ctx.last_icall = target;
    if (IsContinuation(target)) throw X86Resume{target, ctx.esp};
    X86Func* f = FindFunction(target);
    if (!f) throw GuestTrap{target, "indirect call to unknown code"};
    f(ctx, base);
}

void x86_resume(X86Context& ctx, uint8_t* base, const X86Resume& r) {
    ctx.esp = r.esp;
    FindFunction(r.target)(ctx, base);
}

void x86_kernel(X86Context& ctx, uint8_t*, uint32_t ordinal) {
    g_kernel_calls.push_back(ordinal);
    ctx.eax = ordinal;
    ctx.esp += 4;
}

void x86_trap(X86Context&, uint32_t va, const char* what) { throw GuestTrap{va, what}; }
void x86_priv(X86Context&, uint32_t, const char*) {}
uint32_t x86_trace(X86Context&, uint32_t, uint32_t ret) { return ret; }
uint64_t x86_rdtsc() { return 0; }
void x86_cpuid(X86Context& ctx) { ctx.eax = ctx.ebx = ctx.ecx = ctx.edx = 0; }
