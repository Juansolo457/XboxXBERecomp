#include "x86_recomp_shared.h"

X86_FUNC(__imp__sub_00011000) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011000 mov esi, 0x07
	{ ctx.esi = (uint32_t)(0x7u); }
	// 00011005 mov ebx, 0x05
	{ ctx.ebx = (uint32_t)(0x5u); }
	// 0001100A push 0x09
	{ uint32_t v = (uint32_t)(0x9u); X86_PUSH32(v); }
	// 0001100C call 0x00011037
	X86_PUSH32(0x11011u); sub_00011037(ctx, base);
	// 00011011 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011000, __imp__sub_00011000)

X86_FUNC(__imp__sub_00011012) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011012 mov ecx, 0x03
	{ ctx.ecx = (uint32_t)(0x3u); }
	// 00011017 mov edx, 0x04
	{ ctx.edx = (uint32_t)(0x4u); }
	// 0001101C push 0x05
	{ uint32_t v = (uint32_t)(0x5u); X86_PUSH32(v); }
	// 0001101E call 0x00011039
	X86_PUSH32(0x11023u); sub_00011039(ctx, base);
	// 00011023 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011012, __imp__sub_00011012)

X86_FUNC(__imp__sub_00011024) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011024 push 0x02
	{ uint32_t v = (uint32_t)(0x2u); X86_PUSH32(v); }
	// 00011026 push 0x01
	{ uint32_t v = (uint32_t)(0x1u); X86_PUSH32(v); }
	// 00011028 call 0x0001103B
	X86_PUSH32(0x1102Du); sub_0001103B(ctx, base);
	// 0001102D ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_00011024, __imp__sub_00011024)

X86_FUNC(__imp__sub_0001102E) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001102E push 0x06
	{ uint32_t v = (uint32_t)(0x6u); X86_PUSH32(v); }
	// 00011030 call 0x0001103D
	X86_PUSH32(0x11035u); sub_0001103D(ctx, base);
	// 00011035 ret
	ctx.esp += 4; return;
}
X86_WEAK_FUNC(sub_0001102E, __imp__sub_0001102E)

X86_FUNC(__imp__sub_00011037) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011037 int3
	x86_trap(ctx, 0x11037, "int3");
}
X86_WEAK_FUNC(sub_00011037, __imp__sub_00011037)

X86_FUNC(__imp__sub_00011039) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 00011039 int3
	x86_trap(ctx, 0x11039, "int3");
}
X86_WEAK_FUNC(sub_00011039, __imp__sub_00011039)

X86_FUNC(__imp__sub_0001103B) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001103B int3
	x86_trap(ctx, 0x1103B, "int3");
}
X86_WEAK_FUNC(sub_0001103B, __imp__sub_0001103B)

X86_FUNC(__imp__sub_0001103D) {
	X86_LOAD_FLAGS();
	uint32_t ca = 0, cb = 0;
	(void)ca; (void)cb;
	// 0001103D mov eax, 0x70
	{ ctx.eax = (uint32_t)(0x70u); }
	// 00011042 ret 0x04
	ctx.esp += 8; return;
}
X86_WEAK_FUNC(sub_0001103D, __imp__sub_0001103D)

