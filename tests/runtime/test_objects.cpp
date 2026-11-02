#include <atomic>
#include <chrono>
#include <future>
#include <thread>

#include "support.h"

using kernel::Object;
using kernel::ObjectRef;
using kernel::ObjType;

namespace {

constexpr int64_t kForever = INT64_MAX;
constexpr int64_t kPoll = 0;

ObjectRef NewEvent(bool notification, int32_t signal = 0) {
    auto o = std::make_shared<Object>(ObjType::Event);
    o->notification = notification;
    o->signal = signal;
    return o;
}

size_t Waiters(const ObjectRef& o) {
    std::lock_guard<std::mutex> lock(kernel::DispatchLock());
    return o->waiters.size();
}

}  // namespace

TEST_CASE("a synchronization event wakes one wait and resets") {
    auto e = NewEvent(false, 1);
    CHECK(kernel::Wait({e}, false, kPoll, false) == kernel::kWait0);
    CHECK(e->signal == 0);
    CHECK(kernel::Wait({e}, false, kPoll, false) == kernel::kTimeout);
}

TEST_CASE("a notification event stays set until reset") {
    auto e = NewEvent(true, 1);
    CHECK(kernel::Wait({e}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::Wait({e}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::ResetEvent(*e) == 1);
    CHECK(kernel::Wait({e}, false, kPoll, false) == kernel::kTimeout);
}

TEST_CASE("a wait with a relative timeout returns after it") {
    auto e = NewEvent(false);
    auto start = std::chrono::steady_clock::now();
    CHECK(kernel::Wait({e}, false, -100000, false) == kernel::kTimeout);  // 10 ms in 100 ns units
    CHECK(std::chrono::steady_clock::now() - start >= std::chrono::milliseconds(9));
}

TEST_CASE("wait any returns the first signalled object, wait all takes every one") {
    auto a = NewEvent(false), b = NewEvent(false, 1);
    CHECK(kernel::Wait({a, b}, false, kPoll, false) == kernel::kWait0 + 1);
    CHECK(b->signal == 0);

    kernel::SetEvent(*b);
    CHECK(kernel::Wait({a, b}, true, kPoll, false) == kernel::kTimeout);
    CHECK(b->signal == 1);  // a wait all that failed consumes nothing
    kernel::SetEvent(*a);
    CHECK(kernel::Wait({a, b}, true, kPoll, false) == kernel::kWait0);
    CHECK(a->signal == 0);
    CHECK(b->signal == 0);
}

TEST_CASE("a blocked wait sits on its own objects' lists only and leaves when woken") {
    auto e = NewEvent(false), other = NewEvent(false);
    std::atomic<uint32_t> result{0xFFFFFFFF};
    std::thread t([&] { result = kernel::Wait({e}, false, kForever, false); });
    REQUIRE(test::WaitUntil([&] { return Waiters(e) == 1; }));
    CHECK(Waiters(other) == 0);

    kernel::SetEvent(*other);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    CHECK(result == 0xFFFFFFFF);

    kernel::SetEvent(*e);
    t.join();
    CHECK(result == kernel::kWait0);
    CHECK(Waiters(e) == 0);
    CHECK(e->signal == 0);
}

TEST_CASE("one set of a synchronization event releases one of two waits") {
    auto e = NewEvent(false);
    std::atomic<int> done{0};
    auto wait = [&] {
        CHECK(kernel::Wait({e}, false, kForever, false) == kernel::kWait0);
        done++;
    };
    std::thread a(wait), b(wait);
    REQUIRE(test::WaitUntil([&] { return Waiters(e) == 2; }));
    kernel::SetEvent(*e);
    REQUIRE(test::WaitUntil([&] { return done.load() == 1; }));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    CHECK(done == 1);
    kernel::SetEvent(*e);
    a.join();
    b.join();
    CHECK(done == 2);
}

TEST_CASE("a semaphore counts down and refuses a release past its limit") {
    auto s = std::make_shared<Object>(ObjType::Semaphore);
    s->signal = 1;
    s->limit = 2;
    uint32_t status = 1;
    CHECK(kernel::ReleaseSemaphore(*s, 1, status) == 1);
    CHECK(status == kernel::kSuccess);
    kernel::ReleaseSemaphore(*s, 1, status);
    CHECK(status == kernel::kSemaphoreLimitExceeded);
    CHECK(s->signal == 2);
    CHECK(kernel::Wait({s}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::Wait({s}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::Wait({s}, false, kPoll, false) == kernel::kTimeout);
}

TEST_CASE("a mutant nests for its owner and excludes another thread") {
    auto m = std::make_shared<Object>(ObjType::Mutant);
    m->signal = 1;
    CHECK(kernel::Wait({m}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::Wait({m}, false, kPoll, false) == kernel::kWait0);
    CHECK(m->recursion == 2);

    auto other_wait = std::make_shared<std::promise<uint32_t>>();
    auto other_release = std::make_shared<std::promise<uint32_t>>();
    kernel::StartHostThread("mutant test", [=](X86Context&) {
        other_wait->set_value(kernel::Wait({m}, false, kPoll, false));
        uint32_t status = 0;
        kernel::ReleaseMutant(*m, status);
        other_release->set_value(status);
    });
    CHECK(other_wait->get_future().get() == kernel::kTimeout);
    CHECK(other_release->get_future().get() == kernel::kMutantNotOwned);

    uint32_t status = 1;
    kernel::ReleaseMutant(*m, status);
    CHECK(status == kernel::kSuccess);
    CHECK(m->signal == 0);
    kernel::ReleaseMutant(*m, status);
    CHECK(m->signal == 1);
    CHECK(m->owner == 0);
    kernel::ReleaseMutant(*m, status);
    CHECK(status == kernel::kMutantNotOwned);
}

TEST_CASE("a guest dispatcher header becomes the object its type says, and stays that object") {
    uint32_t g = mem::AllocSystem(32);
    kernel::G<uint8_t>(g) = 1;  // SynchronizationEvent
    kernel::W32(g + 4, 1);
    ObjectRef o = kernel::FromPointer(g);
    REQUIRE(o);
    CHECK(o->type == ObjType::Event);
    CHECK_FALSE(o->notification);
    CHECK(kernel::FromPointer(g) == o);
    CHECK(kernel::Wait({o}, false, kPoll, false) == kernel::kWait0);
    CHECK(kernel::R32(g + 4) == 0);  // the guest sees the state in its header

    kernel::SetEvent(*o);
    CHECK(kernel::R32(g + 4) == 1);

    kernel::W32(g + 12, 0);  // the guest initialised the structure again
    ObjectRef again = kernel::FromPointer(g);
    CHECK(again != o);
    CHECK(again->signal == 1);
    mem::FreeSystem(g);
}

TEST_CASE("the other dispatcher types") {
    uint32_t g = mem::AllocSystem(32);
    kernel::G<uint8_t>(g) = 5;  // semaphore: limit after the header
    kernel::W32(g + 4, 3);
    kernel::W32(g + 0x10, 7);
    ObjectRef s = kernel::FromPointer(g);
    CHECK(s->type == ObjType::Semaphore);
    CHECK(s->signal == 3);
    CHECK(s->limit == 7);

    uint32_t t = mem::AllocSystem(32);
    kernel::G<uint8_t>(t) = 8;  // notification timer
    ObjectRef timer = kernel::FromPointer(t);
    CHECK(timer->type == ObjType::Timer);
    CHECK(timer->notification);
    mem::FreeSystem(g);
    mem::FreeSystem(t);
}

TEST_CASE("handles open, duplicate and close") {
    auto e = NewEvent(true);
    uint32_t h = kernel::NewHandle(e);
    CHECK(kernel::FromHandle(h) == e);
    uint32_t d = kernel::DuplicateHandle(h);
    CHECK(d != h);
    CHECK(kernel::CloseHandle(h));
    CHECK_FALSE(kernel::CloseHandle(h));
    CHECK(kernel::FromHandle(h) == nullptr);
    CHECK(kernel::FromHandle(d) == e);
    CHECK(kernel::CloseHandle(d));
}

TEST_CASE("names find an object while it lives") {
    auto e = NewEvent(true);
    kernel::AddNamed("test name", e);
    CHECK(kernel::FindNamed("test name") == e);
    e.reset();
    CHECK(kernel::FindNamed("test name") == nullptr);
}

TEST_CASE("a timer signals when due and a cancelled one does not") {
    auto t = std::make_shared<Object>(ObjType::Timer);
    t->notification = true;
    kernel::SetTimer(t, -20000, 0, 0);  // 2 ms
    CHECK(kernel::Wait({t}, false, -100000000, false) == kernel::kWait0);

    kernel::SetTimer(t, -2000000, 0, 0);  // 200 ms
    CHECK(kernel::CancelTimer(*t));
    CHECK_FALSE(kernel::CancelTimer(*t));
    CHECK(kernel::Wait({t}, false, kPoll, false) == kernel::kTimeout);
}

TEST_CASE("a periodic timer keeps signalling") {
    auto t = std::make_shared<Object>(ObjType::Timer);
    kernel::SetTimer(t, -10000, 1, 0);
    for (int i = 0; i < 3; i++) CHECK(kernel::Wait({t}, false, -100000000, false) == kernel::kWait0);
    CHECK(kernel::CancelTimer(*t));
}

namespace {

std::atomic<int> g_apc_runs{0};
std::atomic<uint32_t> g_apc_arg{0};
std::atomic<uint32_t> g_dpc_context{0};

void ApcRoutine(X86Context& ctx, uint8_t*) {
    g_apc_arg = kernel::R32(ctx.esp + 4);
    g_apc_runs++;
    kernel::Return(ctx, 0, 3);
}

void DpcRoutine(X86Context& ctx, uint8_t*) {
    g_dpc_context = kernel::R32(ctx.esp + 8);
    kernel::Return(ctx, 0, 4);
}

}  // namespace

TEST_CASE("an alertable wait runs a queued APC and returns") {
    auto e = NewEvent(false);
    auto self = std::make_shared<std::promise<kernel::ThreadRef>>();
    auto result = std::make_shared<std::promise<uint32_t>>();
    kernel::StartHostThread("apc test", [=](X86Context&) {
        self->set_value(kernel::CurrentRef());
        result->set_value(kernel::Wait({e}, false, kForever, true));
    });
    kernel::ThreadRef t = self->get_future().get();
    REQUIRE(test::WaitUntil([&] { return Waiters(e) == 1; }));
    kernel::QueueApc(*t, dispatch::HostThunk(ApcRoutine), 0x1234, 0, 0);
    CHECK(result->get_future().get() == kernel::kUserApc);
    CHECK(g_apc_runs == 1);
    CHECK(g_apc_arg == 0x1234);
}

TEST_CASE("a queued DPC runs its routine on the DPC thread") {
    uint32_t dpc = mem::AllocSystem(32);
    kernel::W32(dpc + 0x0C, dispatch::HostThunk(DpcRoutine));  // KDPC.DeferredRoutine
    kernel::W32(dpc + 0x10, 0xABCD);                           // KDPC.DeferredContext
    kernel::QueueDpc(dpc, 1, 2);
    CHECK(test::WaitUntil([&] { return g_dpc_context.load() == 0xABCD; }));
}
