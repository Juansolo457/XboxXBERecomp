#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 cmp eax, 0x03
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(0x3u); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011003 jnbe 0x00011034
	if ((uint32_t)ca > (uint32_t)cb) { goto loc_00011034; }
	// 00011005 jmp [eax*4+0x1100C]
	switch (X86_MEM32((uint32_t)(ctx.eax * 4u + 0x1100Cu))) {
	case 0x1101Cu: goto loc_0001101C;
	case 0x11022u: goto loc_00011022;
	case 0x11028u: goto loc_00011028;
	case 0x1102Eu: goto loc_0001102E;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, X86_MEM32((uint32_t)(ctx.eax * 4u + 0x1100Cu))); return; }
	}
loc_0001101C:
	// 0001101C mov eax, 0x64
	{ ctx.eax = (uint32_t)(0x64u); }
	// 00011021 ret
	ctx.esp += 4; return;
loc_00011022:
	// 00011022 mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 00011027 ret
	ctx.esp += 4; return;
loc_00011028:
	// 00011028 mov eax, 0x66
	{ ctx.eax = (uint32_t)(0x66u); }
	// 0001102D ret
	ctx.esp += 4; return;
loc_0001102E:
	// 0001102E mov eax, 0x67
	{ ctx.eax = (uint32_t)(0x67u); }
	// 00011033 ret
	ctx.esp += 4; return;
loc_00011034:
	// 00011034 mov eax, 0xFFFFFFFF
	{ ctx.eax = (uint32_t)(0xFFFFFFFFu); }
	// 00011039 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

