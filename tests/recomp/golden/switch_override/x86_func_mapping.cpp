#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x11010u;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};
