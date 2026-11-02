#include "guest.h"

TEST_CASE("a byte indexed table goes through both tables") {
    Guest g;
    const uint32_t expected[] = {100, 102, 101, 101, 102, 100};
    for (uint32_t i = 0; i < 6; i++) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == expected[i]);
    }
    g.ctx.eax = 6;
    g.Call(FN_dispatch);
    CHECK(g.ctx.eax == 0xFFFFFFFFu);
}
