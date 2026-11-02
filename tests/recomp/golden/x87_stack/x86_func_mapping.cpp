#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x110CBu;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x11015u, sub_00011015},
	{0x11023u, sub_00011023},
	{0x11028u, sub_00011028},
	{0x11035u, sub_00011035},
	{0x11048u, sub_00011048},
	{0x11059u, sub_00011059},
	{0x11076u, sub_00011076},
	{0x1108Du, sub_0001108D},
	{0x110A6u, sub_000110A6},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};
