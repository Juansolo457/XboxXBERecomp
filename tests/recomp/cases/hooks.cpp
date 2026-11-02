#include "guest.h"

static bool g_result = false;

void hook_before(X86Context& ctx, uint8_t*) { ctx.eax += 10; }
void hook_after(X86Context& ctx, uint8_t*) { ctx.eax *= 2; }
bool hook_leave(X86Context&, uint8_t*) { return g_result; }
bool hook_jump(X86Context&, uint8_t*) { return g_result; }
void hook_flags(X86Context& ctx, uint8_t*) {
    if (g_result) ctx.zf = 0;
}

TEST_CASE("hooks run before and after their instructions") {
    Guest g;
    g.Call(FN_before_after);
    CHECK(g.ctx.eax == 26);  // (1 + 10 + 1 + 1) * 2
}

TEST_CASE("on_true = ret returns from the function") {
    Guest g;
    g_result = true;
    g.Call(FN_leave_fn);
    CHECK(g.ctx.eax == 1);
    g_result = false;
    g.Call(FN_leave_fn);
    CHECK(g.ctx.eax == 2);
}

TEST_CASE("on_true = address jumps within the function") {
    Guest g;
    g_result = true;
    g.Call(FN_jump_fn);
    CHECK(g.ctx.eax == 1);
    g_result = false;
    g.Call(FN_jump_fn);
    CHECK(g.ctx.eax == 18);
}

TEST_CASE("a hook's flag changes reach the next instruction") {
    Guest g;
    g_result = false;
    g.Call(FN_flags_fn);
    CHECK(g.ctx.eax == 1);
    g_result = true;
    g.Call(FN_flags_fn);
    CHECK(g.ctx.eax == 0);
}
