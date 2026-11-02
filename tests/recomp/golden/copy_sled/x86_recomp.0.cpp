#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 and ecx, 0x03
	{ uint32_t a = (uint32_t)(ctx.ecx); uint32_t b = (uint32_t)(0x3u); uint32_t r = (uint32_t)(a & b); ctx.ecx = (uint32_t)(r); cf = 0; of = 0; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011003 neg ecx
	{ uint32_t a = (uint32_t)(ctx.ecx);  uint32_t r = (uint32_t)(0 - a); ctx.ecx = (uint32_t)(r); cf = a != 0; of = a == (uint32_t)(1u << 31); zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011005 add ecx, 0x11010
	{ uint32_t a = (uint32_t)(ctx.ecx); uint32_t b = (uint32_t)(0x11010u); uint32_t r = (uint32_t)(a + b); ctx.ecx = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001100B jmp ecx
	switch (ctx.ecx) {
	case 0x11010u: goto loc_00011010;
	case 0x1100Fu: goto loc_0001100F;
	case 0x1100Eu: goto loc_0001100E;
	case 0x1100Du: goto loc_0001100D;
	default: { X86_SAVE_FLAGS(); x86_icall(ctx, base, ctx.ecx); return; }
	}
loc_0001100D:
	// 0001100D movsb
	{ int32_t d = ctx.df ? -1 : 1; X86_MEM8(ctx.edi) = X86_MEM8(ctx.esi); ctx.esi += d; ctx.edi += d; }
loc_0001100E:
	// 0001100E movsb
	{ int32_t d = ctx.df ? -1 : 1; X86_MEM8(ctx.edi) = X86_MEM8(ctx.esi); ctx.esi += d; ctx.edi += d; }
loc_0001100F:
	// 0001100F movsb
	{ int32_t d = ctx.df ? -1 : 1; X86_MEM8(ctx.edi) = X86_MEM8(ctx.esi); ctx.esi += d; ctx.edi += d; }
loc_00011010:
	// 00011010 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

