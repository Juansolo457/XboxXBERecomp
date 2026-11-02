#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x11059u;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x1101Du, sub_0001101D},
	{0x11031u, sub_00011031},
	{0x1103Fu, sub_0001103F},
	{0x1104Cu, sub_0001104C},
	{0x11053u, sub_00011053},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0x1101Du,
	0,
};
