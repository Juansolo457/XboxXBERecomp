#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	const uint32_t x86_frame = ctx.esp;
	try {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 push 0x1103F
	{ uint32_t v = (uint32_t)(0x1103Fu); X86_PUSH32(v); }
	// 00011005 push fs:[0x00000000]
	{ uint32_t ea = (uint32_t)(ctx.fs_base + 0u); uint32_t v = (uint32_t)(X86_MEM32(ea)); X86_PUSH32(v); }
	// 0001100C mov fs:[0x00000000], esp
	{ uint32_t ea = (uint32_t)(ctx.fs_base + 0u); X86_MEM32(ea) = (uint32_t)(ctx.esp); }
	// 00011013 call 0x00011031
	X86_PUSH32(0x11018u); sub_00011031(ctx, base);
	// 00011018 mov ebx, 0x111
	{ ctx.ebx = (uint32_t)(0x111u); }
	{ X86_SAVE_FLAGS(); sub_0001101D(ctx, base); return; }
	} catch (const X86Resume& r) {
		if (r.esp >= x86_frame) throw;
		x86_resume(ctx, base, r);
	}
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_0001101D) {
	const uint32_t x86_frame = ctx.esp;
	try {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001101D mov ecx, [esp]
	{ uint32_t ea = (uint32_t)(ctx.esp); ctx.ecx = (uint32_t)(X86_MEM32(ea)); }
	// 00011020 mov fs:[0x00000000], ecx
	{ uint32_t ea = (uint32_t)(ctx.fs_base + 0u); X86_MEM32(ea) = (uint32_t)(ctx.ecx); }
	// 00011027 add esp, 0x08
	{ uint32_t a = (uint32_t)(ctx.esp); uint32_t b = (uint32_t)(0x8u); uint32_t r = (uint32_t)(a + b); ctx.esp = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001102A mov eax, 0x777
	{ ctx.eax = (uint32_t)(0x777u); }
	// 0001102F ret
	ctx.esp += 4; return;
	} catch (const X86Resume& r) {
		if (r.esp >= x86_frame) throw;
		x86_resume(ctx, base, r);
	}
}
X86_WEAK_FUNC(sub_0001101D, __imp__sub_0001101D)

X86_FUNC(__imp__sub_00011031) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011031 mov edx, [0x00021044]
	{ uint32_t ea = (uint32_t)(0x21044u); ctx.edx = (uint32_t)(X86_MEM32(ea)); }
	// 00011037 call edx
	{ uint32_t t = ctx.edx; X86_PUSH32(0x11039u); x86_icall(ctx, base, t); }
	// 00011039 add esp, 0x04
	{ uint32_t a = (uint32_t)(ctx.esp); uint32_t b = (uint32_t)(0x4u); uint32_t r = (uint32_t)(a + b); ctx.esp = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001103C jmp eax
	{ uint32_t t = ctx.eax; X86_SAVE_FLAGS(); x86_icall(ctx, base, t); return; }
}
X86_WEAK_FUNC(sub_00011031, __imp__sub_00011031)

X86_FUNC(__imp__sub_0001103F) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001103F mov eax, 0x21004
	{ ctx.eax = (uint32_t)(0x21004u); }
	// 00011044 jmp 0x0001104A
	goto loc_0001104A;
loc_0001104A:
	// 0001104A ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001103F, __imp__sub_0001103F)

X86_FUNC(__imp__sub_0001104C) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001104C mov eax, 0x1101D
	{ ctx.eax = (uint32_t)(0x1101Du); }
	// 00011051 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001104C, __imp__sub_0001104C)

X86_FUNC(__imp__sub_00011053) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011053 mov eax, 0x55
	{ ctx.eax = (uint32_t)(0x55u); }
	// 00011058 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011053, __imp__sub_00011053)

