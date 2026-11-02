#include "guest.h"

TEST_CASE("arithmetic pops back to where it started") {
    Guest g;
    g.Call(FN_add);
    CHECK(g.Read<float>(VA_out32) == 3.75f);
    CHECK(g.ctx.top == 0);
}

TEST_CASE("the stack carries across a call") {
    Guest g;
    g.Call(FN_across_call);
    CHECK(g.Read<double>(VA_out64) == 1.0 + 3.14159265358979323846);
    CHECK(g.ctx.top == 0);
}

TEST_CASE("fistp rounds by the control word") {
    Guest g;
    g.Call(FN_round_nearest);
    CHECK(g.R32(VA_out32) == 2);  // 2.5 to even
    g.Call(FN_round_trunc);
    CHECK(g.R32(VA_out32) == 2);  // 2.7 truncated
    CHECK(g.ctx.fcw == 0x0E7F);
}

TEST_CASE("results round to the precision the control word sets") {
    Guest g;
    g.Call(FN_divide);
    CHECK(g.Read<double>(VA_out64) == 1.0 / 3.0);
    g.ctx.fcw = 0x007F;
    g.Call(FN_divide);
    CHECK(g.Read<double>(VA_out64) == double(float(1.0 / 3.0)));
}

TEST_CASE("compares reach the integer flags") {
    Guest g;
    g.Call(FN_is_below);
    CHECK(g.ctx.eax == 1);
    CHECK(g.ctx.top == 0);
    g.Call(FN_fcomip);
    CHECK(g.ctx.eax == 1);
    CHECK(g.ctx.top == 0);
}

TEST_CASE("a value survives 80 bit memory") {
    Guest g;
    g.Call(FN_tbyte);
    CHECK(g.Read<double>(VA_out64) == 1.5);
}

TEST_CASE("the register file is a ring") {
    Guest g;
    g.Call(FN_wrap);
    CHECK(g.ctx.top == 0);
}
