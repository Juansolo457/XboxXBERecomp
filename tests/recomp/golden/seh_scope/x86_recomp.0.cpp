#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 push 0x21004
	{ uint32_t v = (uint32_t)(0x21004u); X86_PUSH32(v); }
	// 00011005 add esp, 0x04
	{ uint32_t a = (uint32_t)(ctx.esp); uint32_t b = (uint32_t)(0x4u); uint32_t r = (uint32_t)(a + b); ctx.esp = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011008 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 0001100D ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_0001100F) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001100F mov eax, 0x02
	{ ctx.eax = (uint32_t)(0x2u); }
	// 00011014 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001100F, __imp__sub_0001100F)

X86_FUNC(__imp__sub_00011016) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011016 mov eax, 0x03
	{ ctx.eax = (uint32_t)(0x3u); }
	// 0001101B ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011016, __imp__sub_00011016)

