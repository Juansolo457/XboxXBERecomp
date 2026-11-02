#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 add eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a + b); ctx.eax = (uint32_t)(r); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011002 adc edx, ebx
	{ uint32_t a = (uint32_t)(ctx.edx); uint32_t b = (uint32_t)(ctx.ebx); uint32_t c0 = cf; uint32_t r = (uint32_t)(a + b + c0); ctx.edx = (uint32_t)(r); cf = ((uint64_t)a + b + c0) >> 32 & 1; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011004 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011005) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011005 sub eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ctx.eax = (uint32_t)(r); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011007 sbb edx, ebx
	{ uint32_t a = (uint32_t)(ctx.edx); uint32_t b = (uint32_t)(ctx.ebx); uint32_t c0 = cf; uint32_t r = (uint32_t)(a - b - c0); ctx.edx = (uint32_t)(r); cf = (uint64_t)a < (uint64_t)b + c0; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 00011009 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011005, __imp__sub_00011005)

X86_FUNC(__imp__sub_0001100A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001100A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001100C jle 0x00011014
	if ((int32_t)ca <= (int32_t)cb) { goto loc_00011014; }
	// 0001100E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011013 ret
	ctx.esp += 4; return;
loc_00011014:
	// 00011014 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011019 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001100A, __imp__sub_0001100A)

X86_FUNC(__imp__sub_0001101A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001101A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001101C jl 0x00011024
	if ((int32_t)ca < (int32_t)cb) { goto loc_00011024; }
	// 0001101E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011023 ret
	ctx.esp += 4; return;
loc_00011024:
	// 00011024 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011029 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001101A, __imp__sub_0001101A)

X86_FUNC(__imp__sub_0001102A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001102A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001102C jbe 0x00011034
	if ((uint32_t)ca <= (uint32_t)cb) { goto loc_00011034; }
	// 0001102E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011033 ret
	ctx.esp += 4; return;
loc_00011034:
	// 00011034 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011039 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001102A, __imp__sub_0001102A)

X86_FUNC(__imp__sub_0001103A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001103A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001103C jb 0x00011044
	if ((uint32_t)ca < (uint32_t)cb) { goto loc_00011044; }
	// 0001103E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011043 ret
	ctx.esp += 4; return;
loc_00011044:
	// 00011044 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011049 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001103A, __imp__sub_0001103A)

X86_FUNC(__imp__sub_0001104A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001104A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001104C js 0x00011054
	if (((int32_t)((uint32_t)ca - (uint32_t)cb)) < 0) { goto loc_00011054; }
	// 0001104E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011053 ret
	ctx.esp += 4; return;
loc_00011054:
	// 00011054 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011059 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001104A, __imp__sub_0001104A)

X86_FUNC(__imp__sub_0001105A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001105A cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001105C jo 0x00011064
	if (of) { goto loc_00011064; }
	// 0001105E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011063 ret
	ctx.esp += 4; return;
loc_00011064:
	// 00011064 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011069 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001105A, __imp__sub_0001105A)

X86_FUNC(__imp__sub_0001106A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001106A test eax, eax
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.eax); uint32_t r = (uint32_t)(a & b); ca = a; cb = b; cf = 0; of = 0; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 0001106C jle 0x00011074
	if (((int32_t)(ca & cb)) <= 0) { goto loc_00011074; }
	// 0001106E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011073 ret
	ctx.esp += 4; return;
loc_00011074:
	// 00011074 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011079 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001106A, __imp__sub_0001106A)

X86_FUNC(__imp__sub_0001107A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001107A cmp al, cl
	{ uint8_t a = (uint8_t)(((uint8_t)ctx.eax)), b = (uint8_t)(((uint8_t)ctx.ecx)); uint8_t r = (uint8_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 7) & 1; zf = r == 0; sf = (uint32_t)(r >> 7) & 1; pf = X86_PARITY(r); }
	// 0001107C jl 0x00011084
	if ((int8_t)ca < (int8_t)cb) { goto loc_00011084; }
	// 0001107E mov eax, 0x00
	{ ctx.eax = (uint32_t)(0x0u); }
	// 00011083 ret
	ctx.esp += 4; return;
loc_00011084:
	// 00011084 mov eax, 0x01
	{ ctx.eax = (uint32_t)(0x1u); }
	// 00011089 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001107A, __imp__sub_0001107A)

X86_FUNC(__imp__sub_0001108A) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001108A shl eax, cl
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t r = (uint32_t)((uint32_t)a << c); cf = c <= 32 ? ((uint32_t)a >> (32 - c)) & 1 : 0; of = ((uint32_t)(r >> 31) & 1) ^ cf; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); ctx.eax = (uint32_t)(r); } }
	// 0001108C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001108A, __imp__sub_0001108A)

X86_FUNC(__imp__sub_0001108D) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001108D shr eax, cl
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t r = (uint32_t)((uint32_t)a >> c); cf = c <= 32 ? ((uint32_t)a >> (c - 1)) & 1 : 0; of = (uint32_t)(a >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); ctx.eax = (uint32_t)(r); } }
	// 0001108F ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001108D, __imp__sub_0001108D)

X86_FUNC(__imp__sub_00011090) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011090 sar eax, cl
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t r = (uint32_t)((int32_t)(int32_t)a >> c); cf = (uint32_t)((int32_t)(int32_t)a >> (c - 1)) & 1; of = 0; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); ctx.eax = (uint32_t)(r); } }
	// 00011092 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011090, __imp__sub_00011090)

X86_FUNC(__imp__sub_00011093) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011093 rol eax, cl
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t n = c % 32; uint32_t r = n ? (uint32_t)(((uint32_t)a << n) | ((uint32_t)a >> (32 - n))) : a; cf = r & 1; of = ((uint32_t)(r >> 31) & 1) ^ cf; ctx.eax = (uint32_t)(r); } }
	// 00011095 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011093, __imp__sub_00011093)

X86_FUNC(__imp__sub_00011096) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011096 ror eax, cl
	{ uint32_t a = (uint32_t)(ctx.eax); uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t n = c % 32; uint32_t r = n ? (uint32_t)(((uint32_t)a >> n) | ((uint32_t)a << (32 - n))) : a; cf = (uint32_t)(r >> 31) & 1; of = ((uint32_t)(r >> 31) ^ (uint32_t)(r >> 30)) & 1; ctx.eax = (uint32_t)(r); } }
	// 00011098 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011096, __imp__sub_00011096)

X86_FUNC(__imp__sub_00011099) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011099 shld eax, edx, cl
	{ uint32_t a = ctx.eax, b = ctx.edx; uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t r = (uint32_t)((((uint64_t)a << 32) | b) << c >> 32); cf = (a >> (32 - c)) & 1; of = ((r ^ a) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); ctx.eax = (uint32_t)(r); } }
	// 0001109C ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011099, __imp__sub_00011099)

X86_FUNC(__imp__sub_0001109D) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001109D shrd eax, edx, cl
	{ uint32_t a = ctx.eax, b = ctx.edx; uint32_t c = (uint32_t)(((uint8_t)ctx.ecx)) & 31; if (c) { uint32_t r = (uint32_t)((((uint64_t)b << 32) | a) >> c); cf = (a >> (c - 1)) & 1; of = ((r ^ a) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); ctx.eax = (uint32_t)(r); } }
	// 000110A0 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001109D, __imp__sub_0001109D)

X86_FUNC(__imp__sub_000110A1) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110A1 mul ecx
	{ uint64_t p = (uint64_t)ctx.eax * (uint32_t)ctx.ecx; ctx.eax = (uint32_t)p; ctx.edx = (uint32_t)(p >> 32); cf = of = ctx.edx != 0; }
	// 000110A3 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110A1, __imp__sub_000110A1)

X86_FUNC(__imp__sub_000110A4) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110A4 imul ecx
	{ int64_t p = (int64_t)(int32_t)ctx.eax * (int32_t)ctx.ecx; ctx.eax = (uint32_t)p; ctx.edx = (uint32_t)((uint64_t)p >> 32); cf = of = p != (int32_t)p; }
	// 000110A6 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110A4, __imp__sub_000110A4)

X86_FUNC(__imp__sub_000110A7) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110A7 imul eax, ecx
	{ int64_t p = (int64_t)(int32_t)(ctx.eax) * (int64_t)(int32_t)(ctx.ecx); uint32_t r = (uint32_t)p; cf = of = p != (int32_t)r; ctx.eax = (uint32_t)(r); }
	// 000110AA ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110A7, __imp__sub_000110A7)

X86_FUNC(__imp__sub_000110AB) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110AB idiv ecx
	{ int64_t n = (int64_t)(((uint64_t)ctx.edx << 32) | ctx.eax); int32_t d = (int32_t)ctx.ecx; if (!d) x86_trap(ctx, 0x110AB, "divide error"); if (n == INT64_MIN && d == -1) x86_trap(ctx, 0x110AB, "divide error"); int64_t q = n / d; if (q != (int32_t)q) x86_trap(ctx, 0x110AB, "divide error"); ctx.eax = (uint32_t)q; ctx.edx = (uint32_t)(n % d); }
	// 000110AD ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110AB, __imp__sub_000110AB)

X86_FUNC(__imp__sub_000110AE) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110AE div ecx
	{ uint64_t n = ((uint64_t)ctx.edx << 32) | ctx.eax; uint32_t d = (uint32_t)ctx.ecx; if (!d) x86_trap(ctx, 0x110AE, "divide error"); uint64_t q = n / d; if (q > 0xFFFFFFFFull) x86_trap(ctx, 0x110AE, "divide error"); ctx.eax = (uint32_t)q; ctx.edx = (uint32_t)(n % d); }
	// 000110B0 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110AE, __imp__sub_000110AE)

X86_FUNC(__imp__sub_000110B1) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110B1 mov ah, 0x12
	{ ctx.eax = (ctx.eax & 0xFFFF00FFu) | ((uint32_t)(uint8_t)(0x12u) << 8); }
	// 000110B3 mov al, 0x34
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)(0x34u); }
	// 000110B5 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110B1, __imp__sub_000110B1)

X86_FUNC(__imp__sub_000110B6) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110B6 movsx eax, al
	{ ctx.eax = (uint32_t)((uint32_t)(int8_t)(((uint8_t)ctx.eax))); }
	// 000110B9 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110B6, __imp__sub_000110B6)

X86_FUNC(__imp__sub_000110BA) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110BA movzx eax, ax
	{ ctx.eax = (uint32_t)((uint16_t)(((uint16_t)ctx.eax))); }
	// 000110BD ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110BA, __imp__sub_000110BA)

X86_FUNC(__imp__sub_000110BE) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110BE cmp edx, edx
	{ uint32_t a = (uint32_t)(ctx.edx), b = (uint32_t)(ctx.edx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 000110C0 cmovz eax, ecx
	{ if ((uint32_t)ca == (uint32_t)cb) { ctx.eax = (uint32_t)(ctx.ecx); } }
	// 000110C3 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110BE, __imp__sub_000110BE)

X86_FUNC(__imp__sub_000110C4) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110C4 cmp eax, ecx
	{ uint32_t a = (uint32_t)(ctx.eax), b = (uint32_t)(ctx.ecx); uint32_t r = (uint32_t)(a - b); ca = a; cb = b; cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 000110C6 setl al
	{ ctx.eax = (ctx.eax & 0xFFFFFF00u) | (uint8_t)(((int32_t)ca < (int32_t)cb) ? 1u : 0u); }
	// 000110C9 movzx eax, al
	{ ctx.eax = (uint32_t)((uint8_t)(((uint8_t)ctx.eax))); }
	// 000110CC ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110C4, __imp__sub_000110C4)

X86_FUNC(__imp__sub_000110CD) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110CD rep stosd
	{ x86_rep_stos(ctx, base, 4); }
	// 000110CF ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110CD, __imp__sub_000110CD)

X86_FUNC(__imp__sub_000110D0) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110D0 rep movsd
	{ x86_rep_movs(ctx, base, 4); }
	// 000110D2 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110D0, __imp__sub_000110D0)

X86_FUNC(__imp__sub_000110D3) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110D3 repne scasb
	{ int32_t d = ctx.df ? -1 : 1; while (ctx.ecx) { uint8_t a = (uint8_t)ctx.eax, b = X86_MEM8(ctx.edi); ctx.edi += d; uint8_t r = (uint8_t)(a - b); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 7) & 1; zf = r == 0; sf = (uint32_t)(r >> 7) & 1; pf = X86_PARITY(r); ctx.ecx--; if (zf) break; } }
	// 000110D5 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110D3, __imp__sub_000110D3)

X86_FUNC(__imp__sub_000110D6) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110D6 lock xadd [esi], ecx
	{ uint32_t ea = (uint32_t)(ctx.esi); uint32_t b = (uint32_t)(ctx.ecx); uint32_t a = __atomic_fetch_add(X86_PTR(uint32_t, ea), b, __ATOMIC_SEQ_CST); uint32_t r = (uint32_t)(a + b); ctx.ecx = (uint32_t)(a); cf = r < a; of = (uint32_t)(((a ^ r) & (b ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 000110DA ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110D6, __imp__sub_000110D6)

X86_FUNC(__imp__sub_000110DB) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110DB lock cmpxchg [esi], ecx
	{ uint32_t ea = (uint32_t)(ctx.esi); uint32_t x = ctx.eax; uint32_t a = x; bool ok = __atomic_compare_exchange_n(X86_PTR(uint32_t, ea), &a, (uint32_t)(ctx.ecx), false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); if (!ok) { ctx.eax = (uint32_t)(a); } uint32_t r = (uint32_t)(x - a); cf = x < a; of = (uint32_t)(((x ^ a) & (x ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 000110DF ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110DB, __imp__sub_000110DB)

X86_FUNC(__imp__sub_000110E0) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110E0 bsf eax, ecx
	{ uint32_t s = (uint32_t)ctx.ecx; zf = s == 0; if (s) { ctx.eax = (uint32_t)(__builtin_ctz(s)); } }
	// 000110E3 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110E0, __imp__sub_000110E0)

X86_FUNC(__imp__sub_000110E4) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110E4 bswap eax
	{ ctx.eax = (uint32_t)(__builtin_bswap32(ctx.eax)); }
	// 000110E6 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110E4, __imp__sub_000110E4)

X86_FUNC(__imp__sub_000110E7) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110E7 stc
	cf = 1;
	// 000110E8 pushfd
	X86_PUSH32(cf | 2u | pf << 2 | zf << 6 | sf << 7 | 0x200u | ctx.df << 10 | of << 11 | ctx.xflags);
	// 000110E9 pop eax
	{ uint32_t v = X86_MEM32(ctx.esp); ctx.esp += 4; ctx.eax = (uint32_t)(v); }
	// 000110EA ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110E7, __imp__sub_000110E7)

X86_FUNC(__imp__sub_000110EB) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110EB cdq
	ctx.edx = (uint32_t)((int32_t)ctx.eax >> 31);
	// 000110EC ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110EB, __imp__sub_000110EB)

X86_FUNC(__imp__sub_000110ED) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 000110ED push ebp
	{ uint32_t v = (uint32_t)(ctx.ebp); X86_PUSH32(v); }
	// 000110EE mov ebp, esp
	{ ctx.ebp = (uint32_t)(ctx.esp); }
	// 000110F0 sub esp, 0x10
	{ uint32_t a = (uint32_t)(ctx.esp); uint32_t b = (uint32_t)(0x10u); uint32_t r = (uint32_t)(a - b); ctx.esp = (uint32_t)(r); cf = a < b; of = (uint32_t)(((a ^ b) & (a ^ r)) >> 31) & 1; zf = r == 0; sf = (uint32_t)(r >> 31) & 1; pf = X86_PARITY(r); }
	// 000110F3 leave
	ctx.esp = ctx.ebp; ctx.ebp = X86_MEM32(ctx.esp); ctx.esp += 4;
	// 000110F4 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_000110ED, __imp__sub_000110ED)

