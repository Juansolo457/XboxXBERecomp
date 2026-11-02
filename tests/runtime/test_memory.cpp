#include "support.h"

constexpr uint32_t kCommitReserve = mem::kMemCommit | mem::kMemReserve;
constexpr uint32_t kReadWrite = 4;  // PAGE_READWRITE

TEST_CASE("a virtual allocation reserves 64 KB granules and commits all of them") {
    uint32_t base = 0, size = 0x1234;
    REQUIRE(mem::AllocateVirtual(base, size, kCommitReserve, kReadWrite) == 0);
    CHECK((base & 0xFFFF) == 0);
    CHECK(size == 0x10000);
    CHECK(mem::IsCommitted(base));
    CHECK(mem::IsCommitted(base + size - 1));
    kernel::W32(base + size - 4, 0x12345678);  // writable to its end

    uint32_t b = base, s = 0;
    CHECK(mem::FreeVirtual(b, s, mem::kMemRelease) == 0);
    CHECK(b == base);
    CHECK(s == size);
    CHECK_FALSE(mem::IsCommitted(base));
}

TEST_CASE("reserve, commit a part, query, decommit") {
    uint32_t base = 0, size = 0x20000;
    REQUIRE(mem::AllocateVirtual(base, size, mem::kMemReserve, kReadWrite) == 0);
    CHECK_FALSE(mem::IsCommitted(base));

    uint32_t page = base + 0x3000, bytes = 0x1800;
    REQUIRE(mem::AllocateVirtual(page, bytes, mem::kMemCommit, kReadWrite) == 0);
    CHECK(page == base + 0x3000);
    CHECK(bytes == 0x2000);  // whole pages
    CHECK(mem::IsCommitted(base + 0x4FFF));
    CHECK_FALSE(mem::IsCommitted(base + 0x5000));

    mem::RegionInfo r;
    REQUIRE(mem::QueryVirtual(base + 0x3800, r));
    CHECK(r.base == base + 0x3000);
    CHECK(r.alloc_base == base);
    CHECK(r.size == 0x2000);
    CHECK(r.state == mem::kMemCommit);
    REQUIRE(mem::QueryVirtual(base, r));
    CHECK(r.state == mem::kMemReserve);
    CHECK(r.size == 0x3000);

    uint32_t d = base + 0x3000, n = 0x1000;
    CHECK(mem::FreeVirtual(d, n, mem::kMemDecommit) == 0);
    CHECK_FALSE(mem::IsCommitted(base + 0x3000));
    CHECK(mem::IsCommitted(base + 0x4000));

    uint32_t b = base, s = 0;
    CHECK(mem::FreeVirtual(b, s, mem::kMemRelease) == 0);
    REQUIRE(mem::QueryVirtual(base, r));
    CHECK(r.state == 0x10000);  // MEM_FREE
}

TEST_CASE("a fixed reservation over another is refused, a release frees the address") {
    uint32_t base = 0, size = 0x10000;
    REQUIRE(mem::AllocateVirtual(base, size, kCommitReserve, kReadWrite) == 0);
    uint32_t same = base, same_size = 0x10000;
    CHECK(mem::AllocateVirtual(same, same_size, mem::kMemReserve, kReadWrite) == 0xC0000018);  // conflicting addresses

    uint32_t b = base, s = 0;
    REQUIRE(mem::FreeVirtual(b, s, mem::kMemRelease) == 0);
    same = base;
    same_size = 0x10000;
    CHECK(mem::AllocateVirtual(same, same_size, mem::kMemReserve, kReadWrite) == 0);
    CHECK(same == base);
    b = base;
    s = 0;
    CHECK(mem::FreeVirtual(b, s, mem::kMemRelease) == 0);
}

TEST_CASE("freed neighbours merge for a larger allocation") {
    uint32_t a = 0, b = 0, sa = 0x400000, sb = 0x400000;
    REQUIRE(mem::AllocateVirtual(a, sa, mem::kMemReserve, kReadWrite) == 0);
    REQUIRE(mem::AllocateVirtual(b, sb, mem::kMemReserve, kReadWrite) == 0);
    REQUIRE(b == a + 0x400000);
    uint32_t p = a, s = 0;
    REQUIRE(mem::FreeVirtual(p, s, mem::kMemRelease) == 0);
    p = b;
    s = 0;
    REQUIRE(mem::FreeVirtual(p, s, mem::kMemRelease) == 0);
    uint32_t c = 0, sc = 0x800000;
    REQUIRE(mem::AllocateVirtual(c, sc, mem::kMemReserve, kReadWrite) == 0);
    CHECK(c == a);
    p = c;
    s = 0;
    CHECK(mem::FreeVirtual(p, s, mem::kMemRelease) == 0);
}

TEST_CASE("top down allocations come from the high end") {
    uint32_t low = 0, high = 0, s1 = 0x10000, s2 = 0x10000;
    REQUIRE(mem::AllocateVirtual(low, s1, mem::kMemReserve, kReadWrite) == 0);
    REQUIRE(mem::AllocateVirtual(high, s2, mem::kMemReserve | mem::kMemTopDown, kReadWrite) == 0);
    CHECK(high > low);
    CHECK(high + 0x10000 <= mem::kUserHigh);
    uint32_t p = low, s = 0;
    mem::FreeVirtual(p, s, mem::kMemRelease);
    p = high;
    s = 0;
    mem::FreeVirtual(p, s, mem::kMemRelease);
}

TEST_CASE("contiguous memory is zeroed, aligned and shows at both physical windows") {
    uint32_t a = mem::AllocContiguous(0x1234, 0, 0xFFFFFFFF, 0x10000);
    REQUIRE(a);
    CHECK((a & 0xFFFF) == 0);
    CHECK(a >= mem::kContiguousBase + mem::kKernelDataSize);
    CHECK(a < mem::kContiguousBase + mem::kPhysicalSize);
    CHECK(mem::AllocationSize(a) == 0x2000);  // whole pages
    for (uint32_t i = 0; i < 0x2000; i += 4) REQUIRE(kernel::R32(a + i) == 0);

    uint32_t pa = mem::PhysicalFromVirtual(a);
    CHECK(pa == a - mem::kContiguousBase);
    CHECK(mem::VirtualFromPhysical(pa) == a);
    kernel::W32(a, 0x600DF00D);
    CHECK(kernel::R32(mem::kWriteCombineBase + pa) == 0x600DF00D);
    CHECK(mem::PhysicalFromVirtual(mem::kWriteCombineBase + pa) == pa);

    mem::FreeContiguous(a);
    CHECK(mem::AllocationSize(a) == 0);
}

TEST_CASE("contiguous requests keep to their limits and low memory stays free longest") {
    uint32_t any = mem::AllocContiguous(0x1000, 0, 0xFFFFFFFF, 0);
    REQUIRE(any);
    CHECK(mem::PhysicalFromVirtual(any) >= 48u << 20);  // from the top

    uint32_t low = mem::AllocContiguous(0x1000, 0, 0xFFFFFF, 0);  // D3D's report pages: the first 16 MB
    REQUIRE(low);
    CHECK(mem::PhysicalFromVirtual(low) < 16u << 20);

    uint32_t again = mem::AllocContiguous(0x1000, 0, 0xFFFFFFFF, 0);
    mem::FreeContiguous(again);
    CHECK(mem::AllocContiguous(0x1000, 0, 0xFFFFFFFF, 0) == again);  // a freed block comes back

    mem::FreeContiguous(again);
    mem::FreeContiguous(low);
    mem::FreeContiguous(any);
    CHECK(mem::AllocContiguous(mem::kPhysicalSize, 0, 0xFFFFFFFF, 0) == 0);
}

TEST_CASE("system memory is zeroed, aligned and reused") {
    uint32_t a = mem::AllocSystem(100, 64);
    REQUIRE(a);
    CHECK((a & 63) == 0);
    CHECK(a >= mem::kSystemBase);
    CHECK(mem::AllocationSize(a) == 112);  // rounded to 16
    kernel::W32(a, 0xFFFFFFFF);
    mem::FreeSystem(a);
    uint32_t b = mem::AllocSystem(100, 64);
    CHECK(b == a);
    CHECK(kernel::R32(b) == 0);
    mem::FreeSystem(b);
}

TEST_CASE("a stack is committed to its top with a guard below") {
    uint32_t top = mem::AllocStack(0x12345);
    REQUIRE(top);
    CHECK((top & 0xFFFF) == 0);
    CHECK(mem::IsCommitted(top - 4));
    CHECK(mem::IsCommitted(top - 0x20000));
    CHECK_FALSE(mem::IsCommitted(top - 0x20000 - 4));  // the size rounds up to 64 KB granules
    mem::FreeStack(top);
    CHECK_FALSE(mem::IsCommitted(top - 4));
}

TEST_CASE("the counters follow allocations") {
    mem::Stats before = mem::Query();
    uint32_t a = mem::AllocContiguous(0x4000, 0, 0xFFFFFFFF, 0);
    REQUIRE(a);
    mem::Stats during = mem::Query();
    CHECK(during.contiguous_used == before.contiguous_used + 4);
    CHECK(during.available_pages == before.available_pages - 4);
    CHECK(during.total_pages == mem::kPhysicalSize / mem::kPage);
    mem::FreeContiguous(a);
    CHECK(mem::Query().contiguous_used == before.contiguous_used);
}
