#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 push ds
	{ uint16_t v = (uint16_t)(0u); ctx.esp -= 2; X86_MEM16(ctx.esp) = v; }
	// 00011001 push es
	{ uint16_t v = (uint16_t)(0u); ctx.esp -= 2; X86_MEM16(ctx.esp) = v; }
	// 00011002 pop es
	ctx.esp += 4;
	// 00011003 pop ds
	ctx.esp += 4;
	// 00011004 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011005) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011005 mov eax, esp
	{ ctx.eax = (uint32_t)(ctx.esp); }
	// 00011007 push ds
	{ uint16_t v = (uint16_t)(0u); ctx.esp -= 2; X86_MEM16(ctx.esp) = v; }
	// 00011008 sub eax, esp
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.esp); uint32_t r = (uint32_t)(a - b); ctx.eax = (uint32_t)(r); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001100A pop ds
	ctx.esp += 4;
	// 0001100B ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011005, __imp__sub_00011005)

X86_FUNC(__imp__sub_0001100C) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001100C mov eax, esp
	{ ctx.eax = (uint32_t)(ctx.esp); }
	// 0001100E push ax
	{ uint16_t v = (uint16_t)(((uint16_t)ctx.eax)); ctx.esp -= 2; X86_MEM16(ctx.esp) = v; }
	// 00011010 sub eax, esp
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.esp); uint32_t r = (uint32_t)(a - b); ctx.eax = (uint32_t)(r); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011012 pop cx
	{ uint16_t v = X86_MEM16(ctx.esp); ctx.esp += 2; ctx.ecx = (ctx.ecx & 0xFFFF0000u) | (uint16_t)(v); }
	// 00011014 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001100C, __imp__sub_0001100C)

X86_FUNC(__imp__sub_00011015) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011015 mov eax, esp
	{ ctx.eax = (uint32_t)(ctx.esp); }
	// 00011017 push eax
	{ uint32_t v = (uint32_t)(ctx.eax); X86_PUSH32(v); }
	// 00011018 sub eax, esp
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.esp); uint32_t r = (uint32_t)(a - b); ctx.eax = (uint32_t)(r); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001101A pop ecx
	{ uint32_t v = X86_MEM32(ctx.esp); ctx.esp += 4; ctx.ecx = (uint32_t)(v); }
	// 0001101B ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011015, __imp__sub_00011015)

