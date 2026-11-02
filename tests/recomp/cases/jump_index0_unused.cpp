#include "guest.h"

TEST_CASE("a table whose index 0 is no code address starts one entry later") {
    Guest g;
    for (uint32_t i : {1u, 2u}) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == 100 + i);
    }
}
