#include "guest.h"

// Zydis gives a segment operand 16 bits, but its push moves esp by 4 as its pop does. Bink's CPU check pushes both.
TEST_CASE("a segment push takes a four byte slot") {
    Guest g;
    g.CallUnchecked(FN_slot_size);
    CHECK(g.ctx.eax == 4);
}

TEST_CASE("segment pushes and pops balance") {
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
