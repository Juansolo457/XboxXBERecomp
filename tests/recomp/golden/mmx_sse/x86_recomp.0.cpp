#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 movq mm0, [0x00021040]
	{ uint32_t ea = (uint32_t)(0x21040u); ctx.mm[0] = X86_MEM64(ea); }
	// 00011007 paddw mm0, [0x00021048]
	{ uint32_t ea = (uint32_t)(0x21048u); ctx.mm[0] = mmx_map2<uint16_t>(ctx.mm[0], X86_MEM64(ea), [](uint16_t a, uint16_t b) { return a + b; }); }
	// 0001100E movq [0x00021050], mm0
	{ uint32_t ea = (uint32_t)(0x21050u); X86_MEM64(ea) = ctx.mm[0]; }
	// 00011015 emms
	// 00011017 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011018) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011018 movq mm0, [0x00021040]
	{ uint32_t ea = (uint32_t)(0x21040u); ctx.mm[0] = X86_MEM64(ea); }
	// 0001101F paddsw mm0, [0x00021048]
	{ uint32_t ea = (uint32_t)(0x21048u); ctx.mm[0] = mmx_map2<int16_t>(ctx.mm[0], X86_MEM64(ea), [](int16_t a, int16_t b) { return sat<int16_t>((int)a + b); }); }
	// 00011026 movq [0x00021050], mm0
	{ uint32_t ea = (uint32_t)(0x21050u); X86_MEM64(ea) = ctx.mm[0]; }
	// 0001102D emms
	// 0001102F ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011018, __imp__sub_00011018)

X86_FUNC(__imp__sub_00011030) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011030 movq mm0, [0x00021040]
	{ uint32_t ea = (uint32_t)(0x21040u); ctx.mm[0] = X86_MEM64(ea); }
	// 00011037 pmullw mm0, [0x00021048]
	{ uint32_t ea = (uint32_t)(0x21048u); ctx.mm[0] = mmx_map2<int16_t>(ctx.mm[0], X86_MEM64(ea), [](int16_t a, int16_t b) { return (int16_t)((int)a * b); }); }
	// 0001103E psrlq mm0, 0x10
	{ uint64_t n = 16ull; ctx.mm[0] = n >= 64 ? 0 : ctx.mm[0] >> n; }
	// 00011042 movq [0x00021050], mm0
	{ uint32_t ea = (uint32_t)(0x21050u); X86_MEM64(ea) = ctx.mm[0]; }
	// 00011049 emms
	// 0001104B ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011030, __imp__sub_00011030)

X86_FUNC(__imp__sub_0001104C) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001104C pshufw mm0, [0x00021040], 0x1B
	{ uint32_t ea = (uint32_t)(0x21040u); uint16_t a[4], r[4]; uint64_t x = X86_MEM64(ea); std::memcpy(a, &x, 8); r[0] = a[3]; r[1] = a[2]; r[2] = a[1]; r[3] = a[0]; std::memcpy(&ctx.mm[0], r, 8); }
	// 00011054 movq [0x00021050], mm0
	{ uint32_t ea = (uint32_t)(0x21050u); X86_MEM64(ea) = ctx.mm[0]; }
	// 0001105B emms
	// 0001105D ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001104C, __imp__sub_0001104C)

X86_FUNC(__imp__sub_0001105E) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001105E movaps xmm0, [0x00021010]
	{ uint32_t ea = (uint32_t)(0x21010u); ctx.xmm[0] = *X86_PTR(X86Xmm, ea); }
	// 00011065 addps xmm0, [0x00021020]
	{ uint32_t ea = (uint32_t)(0x21020u); X86Xmm v = *X86_PTR(X86Xmm, ea); for (int i = 0; i < 4; i++) { float a = ctx.xmm[0].f[i], b = v.f[i]; (void)a; ctx.xmm[0].f[i] = a + b; } }
	// 0001106C mulps xmm0, xmm0
	{ X86Xmm v = ctx.xmm[0]; for (int i = 0; i < 4; i++) { float a = ctx.xmm[0].f[i], b = v.f[i]; (void)a; ctx.xmm[0].f[i] = a * b; } }
	// 0001106F movaps [0x00021030], xmm0
	{ uint32_t ea = (uint32_t)(0x21030u); *X86_PTR(X86Xmm, ea) = ctx.xmm[0]; }
	// 00011076 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001105E, __imp__sub_0001105E)

X86_FUNC(__imp__sub_00011077) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011077 movaps xmm1, [0x00021010]
	{ uint32_t ea = (uint32_t)(0x21010u); ctx.xmm[1] = *X86_PTR(X86Xmm, ea); }
	// 0001107E shufps xmm1, xmm1, 0x1B
	{ X86Xmm a = ctx.xmm[1], b = ctx.xmm[1]; ctx.xmm[1].u[0] = a.u[3]; ctx.xmm[1].u[1] = a.u[2]; ctx.xmm[1].u[2] = b.u[1]; ctx.xmm[1].u[3] = b.u[0]; }
	// 00011082 movaps [0x00021030], xmm1
	{ uint32_t ea = (uint32_t)(0x21030u); *X86_PTR(X86Xmm, ea) = ctx.xmm[1]; }
	// 00011089 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011077, __imp__sub_00011077)

X86_FUNC(__imp__sub_0001108A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001108A cvttss2si eax, [0x00021058]
	{ uint32_t ea = (uint32_t)(0x21058u); ctx.eax = (uint32_t)((uint32_t)sse_cvtt(X86_MEMF32(ea))); }
	// 00011092 cvtss2si ecx, [0x00021058]
	{ uint32_t ea = (uint32_t)(0x21058u); ctx.ecx = (uint32_t)((uint32_t)sse_cvt(X86_MEMF32(ea))); }
	// 0001109A cvtsi2ss xmm0, ecx
	{ ctx.xmm[0].f[0] = (float)(int32_t)ctx.ecx; }
	// 0001109E movss [0x0002105C], xmm0
	{ uint32_t ea = (uint32_t)(0x2105Cu); X86_MEM32(ea) = ctx.xmm[0].u[0]; }
	// 000110A6 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001108A, __imp__sub_0001108A)

X86_FUNC(__imp__sub_000110A7) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110A7 movss xmm0, dword ptr [0x00021058]
	{ uint32_t ea = (uint32_t)(0x21058u); ctx.xmm[0].u[0] = X86_MEM32(ea); ctx.xmm[0].u[1] = ctx.xmm[0].u[2] = ctx.xmm[0].u[3] = 0; }
	// 000110AF comiss xmm0, [0x00021010]
	{ uint32_t ea = (uint32_t)(0x21010u); float a = ctx.xmm[0].f[0], b = X86_MEMF32(ea); bool u = a != a || b != b; zf = u || a == b; pf = u; cf = u || a < b; of = 0; sf = 0; }
	// 000110B6 setnbe al
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)((!(cf | zf)) ? 1u : 0u); }
	// 000110B9 movzx eax, al
	{ ctx.eax = (uint32_t)((uint8_t)(((uint8_t)ctx.eax))); }
	// 000110BC ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110A7, __imp__sub_000110A7)

