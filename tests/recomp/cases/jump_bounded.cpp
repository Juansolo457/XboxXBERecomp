#include "guest.h"

TEST_CASE("a table bounded by cmp reaches each case and the default") {
    Guest g;
    for (uint32_t i = 0; i < 4; i++) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == 100 + i);
    }
    for (uint32_t i : {4u, 0xFFFFFFFFu}) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == 0xFFFFFFFFu);
    }
}
