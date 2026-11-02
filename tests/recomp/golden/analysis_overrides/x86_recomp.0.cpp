#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011005 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011007) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011007 mov eax, 0x02
	{ ctx.eax = (uint32_t)(0x2u); }
	// 0001100C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011007, __imp__sub_00011007)

X86_FUNC(__imp__sub_00011015) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011015 mov eax, 0x04
	{ ctx.eax = (uint32_t)(0x4u); }
	// 0001101A ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011015, __imp__sub_00011015)

X86_FUNC(__imp__sub_0001101C) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001101C mov eax, 0x05
	{ ctx.eax = (uint32_t)(0x5u); }
	x86_trap(ctx, 0x11021, "fell off decoded code");
}
X86_WEAK_FUNC(sub_0001101C, __imp__sub_0001101C)

