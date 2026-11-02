#include "x86_recomp_shared.h"

const uint32_t X86CodeBase = 0x11000u;
const uint32_t X86CodeEnd = 0x110F5u;
const uint32_t X86ImageBase = 0x10000u;
const uint32_t X86ImageEnd = 0x22000u;
const uint32_t X86EntryPoint = 0x11000u;
const uint32_t X86KernelThunk = 0x21000u;

const X86FuncMapping X86FuncMappings[] = {
	{0x11000u, sub_00011000},
	{0x11005u, sub_00011005},
	{0x1100Au, sub_0001100A},
	{0x1101Au, sub_0001101A},
	{0x1102Au, sub_0001102A},
	{0x1103Au, sub_0001103A},
	{0x1104Au, sub_0001104A},
	{0x1105Au, sub_0001105A},
	{0x1106Au, sub_0001106A},
	{0x1107Au, sub_0001107A},
	{0x1108Au, sub_0001108A},
	{0x1108Du, sub_0001108D},
	{0x11090u, sub_00011090},
	{0x11093u, sub_00011093},
	{0x11096u, sub_00011096},
	{0x11099u, sub_00011099},
	{0x1109Du, sub_0001109D},
	{0x110A1u, sub_000110A1},
	{0x110A4u, sub_000110A4},
	{0x110A7u, sub_000110A7},
	{0x110ABu, sub_000110AB},
	{0x110AEu, sub_000110AE},
	{0x110B1u, sub_000110B1},
	{0x110B6u, sub_000110B6},
	{0x110BAu, sub_000110BA},
	{0x110BEu, sub_000110BE},
	{0x110C4u, sub_000110C4},
	{0x110CDu, sub_000110CD},
	{0x110D0u, sub_000110D0},
	{0x110D3u, sub_000110D3},
	{0x110D6u, sub_000110D6},
	{0x110DBu, sub_000110DB},
	{0x110E0u, sub_000110E0},
	{0x110E4u, sub_000110E4},
	{0x110E7u, sub_000110E7},
	{0x110EBu, sub_000110EB},
	{0x110EDu, sub_000110ED},
	{0, nullptr},
};

const uint32_t X86Continuations[] = {
	0,
};
