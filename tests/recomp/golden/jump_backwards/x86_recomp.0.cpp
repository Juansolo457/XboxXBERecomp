#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 neg eax
	{ uint32_t a = (uint32_t)(ctx.eax);  uint32_t r = (uint32_t)(0 - a); ctx.eax = (uint32_t)(r); cf = a != 0; of = a == (uint32_t)(1u << 31); zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011002 jmp [eax*4+0x11015]
	switch (X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11015u))) {
	case 0x1101Du: goto loc_0001101D;
	case 0x11023u: goto loc_00011023;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11015u))); return; }
	}
loc_0001101D:
	// 0001101D mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 00011022 ret
	ctx.esp += 4; return;
loc_00011023:
	// 00011023 mov eax, 0x66
	{ ctx.eax = (uint32_t)(0x66u); }
	// 00011028 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

