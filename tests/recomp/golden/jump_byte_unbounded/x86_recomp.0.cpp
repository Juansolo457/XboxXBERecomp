#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 movzx eax, byte ptr [eax+0x1101A]
	{ uint32_t ea = (uint32_t)(ctx.eax + 0x1101Au); ctx.eax = (uint32_t)((uint8_t)(X86_MEM8(ea))); }
	// 00011007 jmp [eax*4+0x1100E]
	switch (X86_MEM32((uint32_t)(ctx.eax * 4u + 0x1100Eu))) {
	case 0x1101Eu: goto loc_0001101E;
	case 0x11024u: goto loc_00011024;
	case 0x1102Au: goto loc_0001102A;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, X86_MEM32((uint32_t)(ctx.eax * 4u + 0x1100Eu))); return; }
	}
loc_0001101E:
	// 0001101E mov eax, 0x64
	{ ctx.eax = (uint32_t)(0x64u); }
	// 00011023 ret
	ctx.esp += 4; return;
loc_00011024:
	// 00011024 mov eax, 0x65
	{ ctx.eax = (uint32_t)(0x65u); }
	// 00011029 ret
	ctx.esp += 4; return;
loc_0001102A:
	// 0001102A mov eax, 0x66
	{ ctx.eax = (uint32_t)(0x66u); }
	// 0001102F ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

