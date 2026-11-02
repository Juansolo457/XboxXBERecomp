#include "guest.h"
#include "xdk/hle.h"

static_assert(hle::Find("regs").mapped && hle::Find("regs").n == 3 && hle::Find("regs").pops == 1);
static_assert(!hle::Find("plain").mapped);
static_assert(HAS_regs == 1 && HAS_missing == 0);

static uint32_t g_wrapped_arg = 0;

HLE_STDCALL(regs, 3) { return a[0] * 10000 + a[1] * 100 + a[2]; }
HLE_FASTCALL(fast, 1) { return a[0] * 100 + a[1] * 10 + a[2]; }
HLE_STDCALL(plain, 2) { return a[0] * 10 + a[1]; }
HLE_WRAP(wrapped, 1) { g_wrapped_arg = a[0]; }

TEST_CASE("an [abi] map reads registers and pops only its stack arguments") {
    Guest g;
    g.Call(FN_call_regs);
    CHECK(g.ctx.eax == 70905);
}

TEST_CASE("a fastcall reads ecx, edx, then the stack") {
    Guest g;
    g.Call(FN_call_fast);
    CHECK(g.ctx.eax == 345);
}

TEST_CASE("a name with no map keeps the stack") {
    Guest g;
    g.Call(FN_call_plain);
    CHECK(g.ctx.eax == 12);
}

TEST_CASE("a wrap runs before the recompiled body") {
    Guest g;
    g.Call(FN_call_wrapped);
    CHECK(g_wrapped_arg == 6);
    CHECK(g.ctx.eax == 0x70);
}

TEST_CASE("a symbol the image lacks links and traps") {
    Guest g;
    CHECK_THROWS_AS(g.CallUnchecked(sub_FFFF0000), GuestTrap);
}
