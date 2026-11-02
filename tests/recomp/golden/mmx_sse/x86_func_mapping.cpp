#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x110BDu;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x11018u, sub_00011018},
	{0x11030u, sub_00011030},
	{0x1104Cu, sub_0001104C},
	{0x1105Eu, sub_0001105E},
	{0x11077u, sub_00011077},
	{0x1108Au, sub_0001108A},
	{0x110A7u, sub_000110A7},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};
