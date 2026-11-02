#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x11045u;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x11012u, sub_00011012},
	{0x11024u, sub_00011024},
	{0x1102Eu, sub_0001102E},
	{0x11037u, sub_00011037},
	{0x11039u, sub_00011039},
	{0x1103Bu, sub_0001103B},
	{0x1103Du, sub_0001103D},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};

X86_FUNC(__imp__sub_FFFF0000) { x86_trap(ctx, 0xFFFF0000u, "not in this image"); }
X86_WEAK_FUNC(sub_FFFF0000, __imp__sub_FFFF0000)
