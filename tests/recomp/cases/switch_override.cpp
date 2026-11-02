#include "guest.h"

TEST_CASE("[[switch]] names the targets of a jump with no table") {
    Guest g;
    g.ctx.eax = VA_t0;
    g.Call(FN_dispatch);
    CHECK(g.ctx.eax == 100);
    g.ctx.eax = VA_t1;
    g.Call(FN_dispatch);
    CHECK(g.ctx.eax == 101);
}

TEST_CASE("a target the config does not name is an indirect call") {
    Guest g;
    g.ctx.eax = 0x12345;
    CHECK_THROWS_AS(g.CallUnchecked(FN_dispatch), GuestTrap);
}
