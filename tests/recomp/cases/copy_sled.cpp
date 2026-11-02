#include "guest.h"

TEST_CASE("a jump back into a run of movsb copies that many bytes") {
    for (uint32_t n = 0; n <= 3; n++) {
        Guest g;
        const uint32_t src = Guest::kScratch, dst = Guest::kScratch + 16;
        g.Write<uint32_t>(src, 0x44332211);
        g.ctx.esi = src;
        g.ctx.edi = dst;
        g.ctx.ecx = n;
        g.Call(FN_copy);
        CHECK(g.ctx.esi == src + n);
        CHECK(g.ctx.edi == dst + n);
        const uint32_t mask = n ? 0xFFFFFFFFu >> (32 - n * 8) : 0;
        CHECK(g.R32(dst) == (0x44332211u & mask));
    }
}
