#include "guest.h"

TEST_CASE("a FuncInfo's catch blocks, unwind actions and continuations are found") {
    CHECK(FindFunction(VA_catch_block) != nullptr);
    CHECK(FindFunction(VA_unwind_action) != nullptr);
    CHECK(IsContinuation(VA_continuation));
}

TEST_CASE("a jump to a continuation unwinds to the frame that resumes there") {
    Guest g;
    g.Write<uint32_t>(g.ctx.fs_base, 0xFFFFFFFF);
    g.ctx.ebx = 0;
    g.Call(FN_try_fn);
    CHECK(g.ctx.eax == 0x777);
    CHECK(g.ctx.ebx == 0);  // the path after the call never ran
    CHECK(g.R32(g.ctx.fs_base) == 0xFFFFFFFF);
}

TEST_CASE("an unwind action is callable") {
    Guest g;
    g.CallAddress(VA_unwind_action);
    CHECK(g.ctx.eax == 0x55);
}
