#!/usr/bin/env python3
"""Writes the image_symbols.h a runtime source builds against when there is no image: every symbol it names, absent.

    gen_symbols.py <out header> <source>...
"""
import re
import sys
from pathlib import Path

NAMED = re.compile(r"\b(?:HLE_STDCALL|HLE_FASTCALL|HLE_CDECL|HLE_WRAP|SYM|HAS|GUEST|GUEST_ORIG|GUEST_CALL)\(\s*(\w+)")
HAS = re.compile(r"\bHAS_(\w+)")
SKIP = re.compile(r"#\s*define\s+(?:SYM|HAS)_(\w+)")


def main(argv):
    names = set()
    fallback = set()
    for source in argv[2:]:
        text = Path(source).read_text(encoding="utf-8")
        names |= set(NAMED.findall(text)) | set(HAS.findall(text))
        fallback |= set(SKIP.findall(text))  # the source defines these itself when the config lacks them
    lines = ["#pragma once", '#define IMAGE_XBE "test.xbe"']
    for i, name in enumerate(sorted(names - fallback - {"name"})):
        lines.append(f"#define SYM_{name} {0xFFFF0000 + i:08X}")
        lines.append(f"#define HAS_{name} 0")
    Path(argv[1]).write_text("\n".join(lines) + "\n", newline="\n")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
