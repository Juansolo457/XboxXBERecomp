#include "guest.h"

TEST_CASE("the gap fill finds code nothing calls") {
    Guest g;
    g.Call(FN_start);
    CHECK(g.ctx.eax == 1);
    g.CallAddress(VA_gap_found);
    CHECK(g.ctx.eax == 2);
}

TEST_CASE("data_ranges keep code shaped data out") {
    CHECK(FindFunction(VA_data_start) == nullptr);
    Guest g;
    g.Call(FN_next_fn);
    CHECK(g.ctx.eax == 4);
}

TEST_CASE("[[function]] end limits the decode") {
    Guest g;
    REQUIRE(FindFunction(VA_bounded));
    CHECK_THROWS_AS(g.CallAddress(VA_bounded), GuestTrap);  // it falls off where the config said to stop
}
