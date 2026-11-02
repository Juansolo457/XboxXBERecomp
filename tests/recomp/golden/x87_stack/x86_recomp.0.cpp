#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 fld [0x00021004]
	{ uint32_t ea = (uint32_t)(0x21004u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 00011006 fld [0x00021008]
	{ uint32_t ea = (uint32_t)(0x21008u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 0001100C faddp st1, st0
	{ double s = X87_ST(0); double d = X87_ST(1); X87_ST(1) = x87_prec(ctx, (d + s)); x87_pop(ctx); }
	// 0001100E fstp [0x0002101A]
	{ uint32_t ea = (uint32_t)(0x2101Au); X86_MEMF32(ea) = (float)X87_ST(0); x87_pop(ctx); }
	// 00011014 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011015) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011015 fld1
	{ x87_push(ctx, 1.0); }
	// 00011017 call 0x00011023
	X86_PUSH32(0x1101Cu); sub_00011023(ctx, base);
	// 0001101C fstp [0x0002101E]
	{ uint32_t ea = (uint32_t)(0x2101Eu); X86_MEMF64(ea) = X87_ST(0); x87_pop(ctx); }
	// 00011022 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011015, __imp__sub_00011015)

X86_FUNC(__imp__sub_00011023) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011023 fldpi
	{ x87_push(ctx, 3.14159265358979323846); }
	// 00011025 faddp st1, st0
	{ double s = X87_ST(0); double d = X87_ST(1); X87_ST(1) = x87_prec(ctx, (d + s)); x87_pop(ctx); }
	// 00011027 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011023, __imp__sub_00011023)

X86_FUNC(__imp__sub_00011028) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011028 fld [0x0002100C]
	{ uint32_t ea = (uint32_t)(0x2100Cu); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 0001102E fistp [0x0002101A]
	{ uint32_t ea = (uint32_t)(0x2101Au); X86_MEM32(ea) = (uint32_t)x87_to_i32(ctx, X87_ST(0)); x87_pop(ctx); }
	// 00011034 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011028, __imp__sub_00011028)

X86_FUNC(__imp__sub_00011035) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011035 fldcw [0x00021018]
	{ uint32_t ea = (uint32_t)(0x21018u); ctx.fcw = X86_MEM16(ea); }
	// 0001103B fld [0x00021010]
	{ uint32_t ea = (uint32_t)(0x21010u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 00011041 fistp [0x0002101A]
	{ uint32_t ea = (uint32_t)(0x2101Au); X86_MEM32(ea) = (uint32_t)x87_to_i32(ctx, X87_ST(0)); x87_pop(ctx); }
	// 00011047 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011035, __imp__sub_00011035)

X86_FUNC(__imp__sub_00011048) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011048 fld1
	{ x87_push(ctx, 1.0); }
	// 0001104A fld [0x00021014]
	{ uint32_t ea = (uint32_t)(0x21014u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 00011050 fdivp st1, st0
	{ double s = X87_ST(0); double d = X87_ST(1); X87_ST(1) = x87_prec(ctx, (d / s)); x87_pop(ctx); }
	// 00011052 fstp [0x0002101E]
	{ uint32_t ea = (uint32_t)(0x2101Eu); X86_MEMF64(ea) = X87_ST(0); x87_pop(ctx); }
	// 00011058 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011048, __imp__sub_00011048)

X86_FUNC(__imp__sub_00011059) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011059 fld [0x00021004]
	{ uint32_t ea = (uint32_t)(0x21004u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 0001105F fcomp [0x00021008]
	{ uint32_t ea = (uint32_t)(0x21008u); x87_compare(ctx, X87_ST(0), (double)X86_MEMF32(ea)); x87_pop(ctx); }
	// 00011065 fnstsw ax
	{ ctx.eax = (ctx.eax & 0xFFFF0000u) | x87_status(ctx); }
	// 00011067 sahf
	{ uint32_t h = (ctx.eax >> 8) & 0xFF; sf = (h >> 7) & 1; zf = (h >> 6) & 1; pf = (h >> 2) & 1; cf = h & 1; }
	// 00011068 jb 0x00011070
	if (cf) { goto loc_00011070; }
	// 0001106A mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 0001106F ret
	ctx.esp += 4; return;
loc_00011070:
	// 00011070 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011075 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011059, __imp__sub_00011059)

X86_FUNC(__imp__sub_00011076) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011076 fld [0x00021004]
	{ uint32_t ea = (uint32_t)(0x21004u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 0001107C fld [0x00021008]
	{ uint32_t ea = (uint32_t)(0x21008u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 00011082 fcomip st0, st1
	{ double a = X87_ST(0), b = X87_ST(1); bool u = a != a || b != b; zf = u || a == b; pf = u; cf = u || a < b; of = 0; sf = 0; x87_pop(ctx); }
	// 00011084 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 00011086 setnbe al
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)((!(cf | zf)) ? 1u : 0u); }
	// 00011089 movzx eax, al
	{ ctx.eax = (uint32_t)((uint8_t)(((uint8_t)ctx.eax))); }
	// 0001108C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011076, __imp__sub_00011076)

X86_FUNC(__imp__sub_0001108D) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001108D fld [0x00021004]
	{ uint32_t ea = (uint32_t)(0x21004u); x87_push(ctx, (double)X86_MEMF32(ea)); }
	// 00011093 fstp [0x00021026]
	{ uint32_t ea = (uint32_t)(0x21026u); x87_store80(X86_PTR(uint8_t, ea), X87_ST(0)); x87_pop(ctx); }
	// 00011099 fld [0x00021026]
	{ uint32_t ea = (uint32_t)(0x21026u); x87_push(ctx, x87_load80(X86_PTR(uint8_t, ea))); }
	// 0001109F fstp [0x0002101E]
	{ uint32_t ea = (uint32_t)(0x2101Eu); X86_MEMF64(ea) = X87_ST(0); x87_pop(ctx); }
	// 000110A5 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001108D, __imp__sub_0001108D)

X86_FUNC(__imp__sub_000110A6) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110A6 fld1
	{ x87_push(ctx, 1.0); }
	// 000110A8 fld1
	{ x87_push(ctx, 1.0); }
	// 000110AA fld1
	{ x87_push(ctx, 1.0); }
	// 000110AC fld1
	{ x87_push(ctx, 1.0); }
	// 000110AE fld1
	{ x87_push(ctx, 1.0); }
	// 000110B0 fld1
	{ x87_push(ctx, 1.0); }
	// 000110B2 fld1
	{ x87_push(ctx, 1.0); }
	// 000110B4 fld1
	{ x87_push(ctx, 1.0); }
	// 000110B6 fld1
	{ x87_push(ctx, 1.0); }
	// 000110B8 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110BA fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110BC fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110BE fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110C0 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110C2 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110C4 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110C6 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110C8 fstp st0
	{ X87_ST(0) = X87_ST(0); x87_pop(ctx); }
	// 000110CA ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110A6, __imp__sub_000110A6)

