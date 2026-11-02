#include "guest.h"

namespace {

struct Regs {
    uint32_t eax = 0, ecx = 0, edx = 0, ebx = 0;
};

X86Context Run(X86Func* fn, Regs r) {
    Guest g;
    g.ctx.eax = r.eax;
    g.ctx.ecx = r.ecx;
    g.ctx.edx = r.edx;
    g.ctx.ebx = r.ebx;
    g.Call(fn);
    return g.ctx;
}

uint32_t Eax(X86Func* fn, Regs r) { return Run(fn, r).eax; }

constexpr uint32_t kMinus1 = 0xFFFFFFFFu;

}  // namespace

TEST_CASE("add and adc, sub and sbb chain their carry") {
    auto a = Run(FN_add64, {.eax = kMinus1, .ecx = 1, .edx = 1, .ebx = 2});
    CHECK(a.eax == 0);
    CHECK(a.edx == 4);
    auto s = Run(FN_sub64, {.eax = 0, .ecx = 1, .edx = 5, .ebx = 2});
    CHECK(s.eax == kMinus1);
    CHECK(s.edx == 2);
}

TEST_CASE("cmp then a conditional jump") {
    CHECK(Eax(FN_cmp_jle, {.eax = kMinus1, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_jle, {.eax = 1, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_jle, {.eax = 2, .ecx = 1}) == 0);
    CHECK(Eax(FN_cmp_jl, {.eax = kMinus1, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_jl, {.eax = 1, .ecx = 1}) == 0);
    CHECK(Eax(FN_cmp_jbe, {.eax = kMinus1, .ecx = 1}) == 0);
    CHECK(Eax(FN_cmp_jbe, {.eax = 1, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_jb, {.eax = 0, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_js, {.eax = 0, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_js, {.eax = 1, .ecx = 0}) == 0);
    CHECK(Eax(FN_cmp_jo, {.eax = 0x80000000u, .ecx = 1}) == 1);
    CHECK(Eax(FN_cmp_jo, {.eax = 2, .ecx = 1}) == 0);
}

TEST_CASE("test then jle") {
    CHECK(Eax(FN_test_jle, {.eax = 0}) == 1);
    CHECK(Eax(FN_test_jle, {.eax = kMinus1}) == 1);
    CHECK(Eax(FN_test_jle, {.eax = 1}) == 0);
}

TEST_CASE("a byte compare ignores the upper bits") {
    CHECK(Eax(FN_byte_cmp, {.eax = 0x123456FF, .ecx = 0x00000001}) == 1);
    CHECK(Eax(FN_byte_cmp, {.eax = 0xFFFFFF01, .ecx = 0x00000002}) == 1);
    CHECK(Eax(FN_byte_cmp, {.eax = 0x00000002, .ecx = 0xFFFFFF01}) == 0);
}

TEST_CASE("shifts and rotates") {
    CHECK(Eax(FN_shl, {.eax = 1, .ecx = 4}) == 16);
    CHECK(Eax(FN_shl, {.eax = 1, .ecx = 33}) == 2);  // the count is masked
    CHECK(Eax(FN_shr, {.eax = 0x80000000u, .ecx = 31}) == 1);
    CHECK(Eax(FN_sar, {.eax = 0x80000000u, .ecx = 31}) == kMinus1);
    CHECK(Eax(FN_rol, {.eax = 0x80000001u, .ecx = 1}) == 3);
    CHECK(Eax(FN_ror, {.eax = 0x80000001u, .ecx = 1}) == 0xC0000000u);
    CHECK(Eax(FN_shld, {.eax = 1, .ecx = 1, .edx = 0x80000000u}) == 3);
    CHECK(Eax(FN_shrd, {.eax = 2, .ecx = 1, .edx = 1}) == 0x80000001u);
}

TEST_CASE("multiply and divide") {
    auto m = Run(FN_mul, {.eax = 0x80000000u, .ecx = 4});
    CHECK(m.eax == 0);
    CHECK(m.edx == 2);
    auto i = Run(FN_imul, {.eax = uint32_t(-2), .ecx = 3});
    CHECK(i.eax == uint32_t(-6));
    CHECK(i.edx == kMinus1);
    CHECK(Eax(FN_imul2, {.eax = uint32_t(-2), .ecx = 3}) == uint32_t(-6));
    auto d = Run(FN_idiv, {.eax = uint32_t(-7), .ecx = 2, .edx = kMinus1});
    CHECK(d.eax == uint32_t(-3));
    CHECK(d.edx == kMinus1);
    auto u = Run(FN_div, {.eax = 0, .ecx = 2, .edx = 1});
    CHECK(u.eax == 0x80000000u);
    CHECK(u.edx == 0);
}

TEST_CASE("a divide error traps") {
    Guest g;
    g.ctx.ecx = 0;
    CHECK_THROWS_AS(g.CallUnchecked(FN_div), GuestTrap);
    g.ctx.eax = 0;
    g.ctx.edx = 2;
    g.ctx.ecx = 2;  // a quotient past 32 bits
    CHECK_THROWS_AS(g.CallUnchecked(FN_div), GuestTrap);
}

TEST_CASE("partial registers keep the rest") {
    CHECK(Eax(FN_high8, {.eax = 0xAABBCCDD}) == 0xAABB1234u);
    CHECK(Eax(FN_movsx, {.eax = 0x80}) == 0xFFFFFF80u);
    CHECK(Eax(FN_movzx, {.eax = 0xFFFF1234}) == 0x1234);
}

TEST_CASE("cmov and setcc") {
    CHECK(Eax(FN_cmove, {.eax = 1, .ecx = 2}) == 2);
    CHECK(Eax(FN_setl, {.eax = kMinus1, .ecx = 0}) == 1);
    CHECK(Eax(FN_setl, {.eax = 1, .ecx = 0}) == 0);
}

TEST_CASE("rep stosd fills and advances") {
    Guest g;
    g.ctx.edi = Guest::kScratch;
    g.ctx.eax = 0x11223344;
    g.ctx.ecx = 3;
    g.Call(FN_rep_stosd);
    CHECK(g.R32(Guest::kScratch) == 0x11223344);
    CHECK(g.R32(Guest::kScratch + 8) == 0x11223344);
    CHECK(g.R32(Guest::kScratch + 12) == 0);
    CHECK(g.ctx.edi == Guest::kScratch + 12);
    CHECK(g.ctx.ecx == 0);
}

TEST_CASE("rep movsd over its own output repeats the first element") {
    Guest g;
    for (uint32_t i = 0; i < 4; i++) g.Write<uint32_t>(Guest::kScratch + i * 4, i + 1);
    g.ctx.esi = Guest::kScratch;
    g.ctx.edi = Guest::kScratch + 4;
    g.ctx.ecx = 3;
    g.Call(FN_rep_movsd);
    for (uint32_t i = 0; i < 4; i++) CHECK(g.R32(Guest::kScratch + i * 4) == 1);
}

TEST_CASE("repne scasb stops at the match") {
    Guest g;
    g.Write<uint32_t>(Guest::kScratch, 0x00636261);
    g.ctx.edi = Guest::kScratch;
    g.ctx.eax = 0;
    g.ctx.ecx = kMinus1;
    g.Call(FN_repne_scasb);
    CHECK(g.ctx.edi == Guest::kScratch + 4);
    CHECK(g.ctx.ecx == kMinus1 - 4);
}

TEST_CASE("locked read modify writes") {
    Guest g;
    g.Write<uint32_t>(Guest::kScratch, 5);
    g.ctx.esi = Guest::kScratch;
    g.ctx.ecx = 3;
    g.Call(FN_lock_xadd);
    CHECK(g.R32(Guest::kScratch) == 8);
    CHECK(g.ctx.ecx == 5);
    g.ctx.eax = 8;
    g.ctx.ecx = 9;
    g.Call(FN_lock_cmpxchg);
    CHECK(g.R32(Guest::kScratch) == 9);
    g.ctx.eax = 1;
    g.ctx.ecx = 7;
    g.Call(FN_lock_cmpxchg);
    CHECK(g.R32(Guest::kScratch) == 9);
    CHECK(g.ctx.eax == 9);
}

TEST_CASE("the rest") {
    CHECK(Eax(FN_bsf, {.ecx = 8}) == 3);
    CHECK(Eax(FN_bswap, {.eax = 0x11223344}) == 0x44332211);
    CHECK(Eax(FN_pushfd_popfd, {}) == 0x203);  // CF, the always set bit, IF
    CHECK(Run(FN_cdq, {.eax = kMinus1}).edx == kMinus1);
    auto l = Run(FN_leave_fn, {});
    CHECK(l.ebp == 0);
}
