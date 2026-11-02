#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 call 0x00011005
	X86_PUSH32(0x11005u);
	{ X86_SAVE_FLAGS(); sub_00011005(ctx, base); return; }
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011005) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011005 pop eax
	{ uint32_t v = X86_MEM32(ctx.esp); ctx.esp += 4; ctx.eax = (uint32_t)(v); }
	// 00011006 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011005, __imp__sub_00011005)

X86_FUNC(__imp__sub_00011007) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011007 call ecx
	{ uint32_t t = ctx.ecx; X86_PUSH32(0x11009u); x86_icall(ctx, base, t); }
	// 00011009 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011007, __imp__sub_00011007)

X86_FUNC(__imp__sub_0001100A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001100A mov eax, 0x2A
	{ ctx.eax = (uint32_t)(0x2Au); }
	// 0001100F ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001100A, __imp__sub_0001100A)

X86_FUNC(__imp__sub_00011010) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011010 jmp 0x0001100A
	X86_BARRIER();
	{ X86_SAVE_FLAGS(); sub_0001100A(ctx, base); return; }
}
X86_WEAK_FUNC(sub_00011010, __imp__sub_00011010)

X86_FUNC(__imp__sub_00011015) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011015 jmp ecx
	{ uint32_t t = ctx.ecx; X86_SAVE_FLAGS(); x86_icall(ctx, base, t); return; }
}
X86_WEAK_FUNC(sub_00011015, __imp__sub_00011015)

X86_FUNC(__imp__sub_00011018) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011018 xor eax, eax
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.eax); uint32_t r = (uint32_t)(a ^ b); ctx.eax = (uint32_t)(r); cf = 0; of = 0; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
loc_0001101A:
	// 0001101A inc eax
	{ uint32_t a = (uint32_t)(ctx.eax);  uint32_t r = (uint32_t)(a + 1); ctx.eax = (uint32_t)(r); of = r == (uint32_t)(1u << 31); zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001101B loop 0x0001101A
	if (--ctx.ecx != 0) { X86_BARRIER(); goto loc_0001101A; }
	// 0001101D ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011018, __imp__sub_00011018)

X86_FUNC(__imp__sub_0001101E) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001101E call [0x00021000]
	X86_PUSH32(0x11024u); x86_kernel(ctx, base, 49);
	// 00011024 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001101E, __imp__sub_0001101E)

X86_FUNC(__imp__sub_00011025) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011025 jmp [0x00021004]
	x86_kernel(ctx, base, 156); return;
}
X86_WEAK_FUNC(sub_00011025, __imp__sub_00011025)

X86_FUNC(__imp__sub_0001102C) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001102C ud2
	x86_trap(ctx, 0x1102C, "ud2");
}
X86_WEAK_FUNC(sub_0001102C, __imp__sub_0001102C)

X86_FUNC(__imp__sub_0001102F) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001102F cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011031 jmp 0x00011036
	{ X86_SAVE_FLAGS(); sub_00011036(ctx, base); return; }
}
X86_WEAK_FUNC(sub_0001102F, __imp__sub_0001102F)

X86_FUNC(__imp__sub_00011036) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011036 setz al
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)((zf) ? 1u : 0u); }
	// 00011039 movzx eax, al
	{ ctx.eax = (uint32_t)((uint8_t)(((uint8_t)ctx.eax))); }
	// 0001103C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011036, __imp__sub_00011036)

X86_FUNC(__imp__sub_00011043) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	goto loc_00011043;
loc_0001103D:
	// 0001103D mov eax, 0x09
	{ ctx.eax = (uint32_t)(0x9u); }
	// 00011042 ret
	ctx.esp += 4; return;
loc_00011043:
	// 00011043 jmp 0x0001103D
	X86_BARRIER();
	goto loc_0001103D;
}
X86_WEAK_FUNC(sub_00011043, __imp__sub_00011043)

