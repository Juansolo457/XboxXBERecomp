#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x11045u;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x11005u, sub_00011005},
	{0x11007u, sub_00011007},
	{0x1100Au, sub_0001100A},
	{0x11010u, sub_00011010},
	{0x11015u, sub_00011015},
	{0x11018u, sub_00011018},
	{0x1101Eu, sub_0001101E},
	{0x11025u, sub_00011025},
	{0x1102Cu, sub_0001102C},
	{0x1102Fu, sub_0001102F},
	{0x11036u, sub_00011036},
	{0x11043u, sub_00011043},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};
