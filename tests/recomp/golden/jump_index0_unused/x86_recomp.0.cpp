#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 jmp [eax*4+0x11007]
	switch (X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11007u))) {
	case 0x11017u: goto loc_00011017;
	case 0x1101Du: goto loc_0001101D;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11007u))); return; }
	}
loc_00011017:
	// 00011017 mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 0001101C ret
	ctx.esp += 4; return;
loc_0001101D:
	// 0001101D mov eax, 0x66
	{ ctx.eax = (uint32_t)(0x66u); }
	// 00011022 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

