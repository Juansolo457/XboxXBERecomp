#include "guest.h"

TEST_CASE("neg idx indexes a table that ends at its base") {
    Guest g;
    for (uint32_t i : {1u, 2u}) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == 100 + i);
    }
}
