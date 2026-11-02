#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 cmp eax, 0x05
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(0x5u); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011003 jnbe 0x00011037
	if ((uint32_t)ca > (uint32_t)cb) { goto loc_00011037; }
	// 00011005 movzx eax, byte ptr [eax+0x1101F]
	{ uint32_t ea = (uint32_t)(ctx.eax + 0x1101Fu); ctx.eax = (uint32_t)((uint8_t)(X86_MEM8(ea))); }
	// 0001100C jmp [eax*4+0x11013]
	switch (X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11013u))) {
	case 0x11025u: goto loc_00011025;
	case 0x1102Bu: goto loc_0001102B;
	case 0x11031u: goto loc_00011031;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, X86_MEM32((uint32_t)(ctx.eax * 4u + 0x11013u))); return; }
	}
loc_00011025:
	// 00011025 mov eax, 0x64
	{ ctx.eax = (uint32_t)(0x64u); }
	// 0001102A ret
	ctx.esp += 4; return;
loc_0001102B:
	// 0001102B mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 00011030 ret
	ctx.esp += 4; return;
loc_00011031:
	// 00011031 mov eax, 0x66
	{ ctx.eax = (uint32_t)(0x66u); }
	// 00011036 ret
	ctx.esp += 4; return;
loc_00011037:
	// 00011037 mov eax, 0xFFFFFFFF
	{ ctx.eax = (uint32_t)(0xFFFFFFFFu); }
	// 0001103C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

