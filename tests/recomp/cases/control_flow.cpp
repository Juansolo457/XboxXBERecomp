#include "guest.h"

TEST_CASE("call $+5 only pushes eip") {
    Guest g;
    g.Call(FN_call_next);
    CHECK(g.ctx.eax == VA_after_call);
}

TEST_CASE("indirect calls go through the function table") {
    Guest g;
    g.ctx.ecx = VA_callee;
    g.Call(FN_indirect_call);
    CHECK(g.ctx.eax == 42);
    CHECK(g.ctx.last_icall == VA_callee);
}

TEST_CASE("an indirect call to unknown code traps") {
    Guest g;
    g.ctx.ecx = 0x12345;
    CHECK_THROWS_AS(g.CallUnchecked(FN_indirect_call), GuestTrap);
}

TEST_CASE("tail jumps leave the stack to the callee") {
    Guest g;
    g.Call(FN_tail);
    CHECK(g.ctx.eax == 42);
    g.ctx.eax = 0;
    g.ctx.ecx = VA_callee;
    g.Call(FN_indirect_tail);
    CHECK(g.ctx.eax == 42);
}

TEST_CASE("loop counts ecx down") {
    Guest g;
    g.ctx.ecx = 3;
    g.Call(FN_loop_fn);
    CHECK(g.ctx.eax == 3);
    CHECK(g.ctx.ecx == 0);
}

TEST_CASE("the kernel thunk's slots turn into ordinals") {
    Guest g;
    g.Call(FN_kernel_call);
    g.Call(FN_kernel_tail);
    REQUIRE(g_kernel_calls.size() == 2);
    CHECK(g_kernel_calls[0] == 49);
    CHECK(g_kernel_calls[1] == 156);
}

TEST_CASE("ud2 traps") {
    Guest g;
    CHECK_THROWS_AS(g.CallUnchecked(FN_trap), GuestTrap);
}

TEST_CASE("flags live across a tail call") {
    Guest g;
    g.ctx.eax = 5;
    g.ctx.ecx = 5;
    g.Call(FN_flags_tail);
    CHECK(g.ctx.eax == 1);
    g.ctx.eax = 5;
    g.ctx.ecx = 6;
    g.Call(FN_flags_tail);
    CHECK(g.ctx.eax == 0);
}

TEST_CASE("code below the entry is entered by label") {
    Guest g;
    g.Call(FN_shared_entry);
    CHECK(g.ctx.eax == 9);
    CHECK(FindFunction(VA_shared_tail) == nullptr);
}
