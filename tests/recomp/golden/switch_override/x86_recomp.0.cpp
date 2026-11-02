#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 jmp eax
	switch (ctx.eax) {
	case 0x11003u: goto loc_00011003;
	case 0x1100Au: goto loc_0001100A;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, ctx.eax); return; }
	}
loc_00011003:
	// 00011003 mov eax, 0x64
	{ ctx.eax = (uint32_t)(0x64u); }
	// 00011008 ret
	ctx.esp += 4; return;
loc_0001100A:
	// 0001100A mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 0001100F ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

