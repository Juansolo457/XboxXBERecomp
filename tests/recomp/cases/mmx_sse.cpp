#include "guest.h"

static void CheckFloats(const Guest& g, uint32_t va, float a, float b, float c, float d) {
    CHECK(g.Read<float>(va) == a);
    CHECK(g.Read<float>(va + 4) == b);
    CHECK(g.Read<float>(va + 8) == c);
    CHECK(g.Read<float>(va + 12) == d);
}

TEST_CASE("paddw wraps each lane") {
    Guest g;
    g.Call(FN_paddw);
    CHECK(g.Read<uint64_t>(VA_out64) == 0x00007FFF80000003ull);
}

TEST_CASE("paddsw saturates each lane") {
    Guest g;
    g.Call(FN_paddsw);
    CHECK(g.Read<uint64_t>(VA_out64) == 0x000080007FFF0003ull);
}

TEST_CASE("pmullw keeps the low half, psrlq shifts the whole register") {
    Guest g;
    g.Call(FN_pmullw_psrlq);
    CHECK(g.Read<uint64_t>(VA_out64) == 0x0000FFFF80007FFFull);
}

TEST_CASE("pshufw picks lanes by its immediate") {
    Guest g;
    g.Call(FN_pshufw);
    CHECK(g.Read<uint64_t>(VA_out64) == 0x00017FFF8000FFFFull);
}

TEST_CASE("addps and mulps work lane by lane") {
    Guest g;
    g.Call(FN_addps_mulps);
    CheckFloats(g, VA_out128, 2.25f, 5.0625f, 25.0f, 9.0f);
}

TEST_CASE("shufps picks lanes by its immediate") {
    Guest g;
    g.Call(FN_shufps);
    CheckFloats(g, VA_out128, 4.0f, 3.0f, 2.0f, 1.0f);
}

TEST_CASE("float to integer conversions truncate or round") {
    Guest g;
    g.Call(FN_cvt);
    CHECK(g.ctx.eax == 2);
    CHECK(g.ctx.ecx == 3);
    CHECK(g.Read<float>(VA_out32) == 3.0f);
}

TEST_CASE("comiss sets the integer flags") {
    Guest g;
    g.Call(FN_comiss);
    CHECK(g.ctx.eax == 1);
}
