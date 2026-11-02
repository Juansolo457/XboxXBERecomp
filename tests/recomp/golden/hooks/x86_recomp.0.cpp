#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	X86_SAVE_FLAGS();
	hook_before(ctx, base); cf = ctx.cf; zf = ctx.zf; sf = ctx.sf; of = ctx.of; pf = ctx.pf;
	// 00011005 inc eax
	{ uint32_t a = (uint32_t)(ctx.eax);  uint32_t r = (uint32_t)(a + 1); ctx.eax = (uint32_t)(r); of = r == (uint32_t)(1u << 31); zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011006 inc eax
	{ uint32_t a = (uint32_t)(ctx.eax);  uint32_t r = (uint32_t)(a + 1); ctx.eax = (uint32_t)(r); of = r == (uint32_t)(1u << 31); zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	X86_SAVE_FLAGS();
	hook_after(ctx, base); cf = ctx.cf; zf = ctx.zf; sf = ctx.sf; of = ctx.of; pf = ctx.pf;
	// 00011007 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011008) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011008 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	X86_SAVE_FLAGS();
	{ bool hook = hook_leave(ctx, base); cf = ctx.cf; zf = ctx.zf; sf = ctx.sf; of = ctx.of; pf = ctx.pf; if (hook) { ctx.esp += 4; return; } }
	// 0001100D mov eax, 0x02
	{ ctx.eax = (uint32_t)(0x2u); }
	// 00011012 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011008, __imp__sub_00011008)

X86_FUNC(__imp__sub_00011013) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011013 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	X86_SAVE_FLAGS();
	{ bool hook = hook_jump(ctx, base); cf = ctx.cf; zf = ctx.zf; sf = ctx.sf; of = ctx.of; pf = ctx.pf; if (hook) { goto loc_00011020; } }
	// 00011018 mov eax, 0x02
	{ ctx.eax = (uint32_t)(0x2u); }
	// 0001101D add eax, 0x10
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(0x10u); uint32_t r = (uint32_t)(a + b); ctx.eax = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
loc_00011020:
	// 00011020 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011013, __imp__sub_00011013)

X86_FUNC(__imp__sub_00011021) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011021 xor eax, eax
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.eax); uint32_t r = (uint32_t)(a ^ b); ctx.eax = (uint32_t)(r); cf = 0; of = 0; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	X86_SAVE_FLAGS();
	hook_flags(ctx, base); cf = ctx.cf; zf = ctx.zf; sf = ctx.sf; of = ctx.of; pf = ctx.pf;
	// 00011023 setz al
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)((zf) ? 1u : 0u); }
	// 00011026 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011021, __imp__sub_00011021)

