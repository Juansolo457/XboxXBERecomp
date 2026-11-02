#include "guest.h"

// SDK bug at f7b13fd: push ds and push es move esp by 2 while pop moves it by 4, because Zydis gives a segment operand
// 16 bits. Bink's CPU check pushes both. should_fail keeps the suite green and fails it when the bug is fixed, at which
// point the decorators come off.
TEST_CASE("a segment push takes a four byte slot" * doctest::should_fail()) {
    Guest g;
    g.CallUnchecked(FN_slot_size);
    CHECK(g.ctx.eax == 4);
}

TEST_CASE("segment pushes and pops balance" * doctest::should_fail()) {
    Guest g;
    g.Call(FN_balance);
}

TEST_CASE("a 16 bit register push takes two bytes") {
    Guest g;
    g.Call(FN_word_push);
    CHECK(g.ctx.eax == 2);
}

TEST_CASE("a 32 bit register push takes four bytes") {
    Guest g;
    g.Call(FN_dword_push);
    CHECK(g.ctx.eax == 4);
}
