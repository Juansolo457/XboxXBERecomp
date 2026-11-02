#pragma once
#include "cpu/x86_ops.h"

X86_EXTERN_FUNC(sub_00011000);
X86_EXTERN_FUNC(sub_00011008);
X86_EXTERN_FUNC(sub_00011013);
X86_EXTERN_FUNC(sub_00011021);
X86_HOST_API void hook_after(X86Context& ctx, uint8_t* base);
X86_HOST_API void hook_before(X86Context& ctx, uint8_t* base);
X86_HOST_API void hook_flags(X86Context& ctx, uint8_t* base);
X86_HOST_API bool hook_jump(X86Context& ctx, uint8_t* base);
X86_HOST_API bool hook_leave(X86Context& ctx, uint8_t* base);
