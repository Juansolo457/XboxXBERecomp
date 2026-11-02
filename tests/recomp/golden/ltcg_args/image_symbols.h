#pragma once
#define IMAGE_XBE "default.xbe"
#define SYM_fast 00011039
#define HAS_fast 1
#define SYM_missing FFFF0000
#define HAS_missing 0
#define SYM_plain 0001103B
#define HAS_plain 1
#define SYM_regs 00011037
#define HAS_regs 1
#define SYM_wrapped 0001103D
#define HAS_wrapped 1
#define IMAGE_ABI(X) \
	X(regs, esi, stack, ebx)
