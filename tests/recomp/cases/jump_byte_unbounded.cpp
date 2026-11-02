#include "guest.h"

TEST_CASE("a byte table with no cmp runs while its bytes index the dword table") {
    Guest g;
    const uint32_t expected[] = {102, 100, 101};
    for (uint32_t i = 0; i < 3; i++) {
        g.ctx.eax = i;
        g.Call(FN_dispatch);
        CHECK(g.ctx.eax == expected[i]);
    }
}
