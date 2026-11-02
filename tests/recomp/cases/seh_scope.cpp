#include "guest.h"

TEST_CASE("a scope table's filter and handler become functions through their pointers") {
    Guest g;
    g.Call(FN_guarded);
    CHECK(g.ctx.eax == 1);
    g.CallAddress(VA_filter);
    CHECK(g.ctx.eax == 2);
    g.CallAddress(VA_handler);
    CHECK(g.ctx.eax == 3);
}
