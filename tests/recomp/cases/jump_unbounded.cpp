#include "guest.h"

TEST_CASE("a table with no cmp runs while its entries are code") {
    Guest g;
    for (uint32_t i = 0; i < 3; i++) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == 100 + i);
    }
}
