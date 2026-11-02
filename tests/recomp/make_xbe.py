#!/usr/bin/env python3
"""Writes a minimal XBE around hand assembled x86, with the config xbe_recomp reads.

    make_xbe.py <case> <out folder>   default.xbe, config/game.toml, case_symbols.h
    make_xbe.py --list                the case names

The header holds what runtime/xbe/xbe.h parses and nothing more. The image has two sections: .text, the case's code,
and .rdata, the kernel thunk followed by the case's data. No library or game bytes go in.
"""
import struct
import sys
from pathlib import Path

BASE = 0x10000
HEADER_SIZE = 0x1000
TEXT_VA = 0x11000
RDATA_VA = 0x21000  # a fixed distance, so code can name data before the code is laid out
RETAIL_ENTRY_KEY = 0xA8FC57AB
RETAIL_THUNK_KEY = 0x5B6D40B6


class L:
    """A label's address as a dword."""
    def __init__(self, name):
        self.name = name


class R:
    """A label as a rel32 from the end of the field."""
    def __init__(self, name):
        self.name = name


class R8:
    """A label as a rel8 from the end of the field."""
    def __init__(self, name):
        self.name = name


class D(int):
    """A dword."""


class F32(float):
    """A float."""


class F64(float):
    """A double."""


class Section:
    def __init__(self, image, va):
        self.image = image
        self.va = va
        self.buf = bytearray()
        self.fixups = []

    def here(self):
        return self.va + len(self.buf)

    def label(self, name):
        if name in self.image.labels:
            raise ValueError(f"label {name} defined twice")
        self.image.labels[name] = self.here()

    def fn(self, name):
        """A function the test calls: the config declares it and case_symbols.h names it."""
        self.label(name)
        self.image.entries.append(name)

    def emit(self, *parts):
        """Hex strings and L, R, R8, D, F32, F64 fields, in order."""
        for p in parts:
            if isinstance(p, str):
                self.buf += bytes.fromhex(p)
            elif isinstance(p, D):
                self.buf += struct.pack("<I", int(p) & 0xFFFFFFFF)
            elif isinstance(p, F32):
                self.buf += struct.pack("<f", float(p))
            elif isinstance(p, F64):
                self.buf += struct.pack("<d", float(p))
            elif isinstance(p, (L, R, R8)):
                self.fixups.append((len(self.buf), p))
                self.buf += bytes(1 if isinstance(p, R8) else 4)
            elif isinstance(p, (bytes, bytearray)):
                self.buf += p
            else:
                raise TypeError(p)

    def resolve(self):
        for offset, f in self.fixups:
            target = self.image.labels[f.name]
            if isinstance(f, L):
                struct.pack_into("<I", self.buf, offset, target)
            elif isinstance(f, R):
                struct.pack_into("<i", self.buf, offset, target - (self.va + offset + 4))
            else:
                struct.pack_into("<b", self.buf, offset, target - (self.va + offset + 1))


class Image:
    def __init__(self, ordinals=()):
        self.labels = {}
        self.entries = []
        self.text = Section(self, TEXT_VA)
        self.rdata = Section(self, RDATA_VA)
        self.rdata.label("kernel_thunk")
        for i, ordinal in enumerate(ordinals):
            self.rdata.label(f"thunk_{ordinal}")
            self.rdata.emit(D(0x80000000 | ordinal))
        self.rdata.emit(D(0))

    def write(self):
        for s in (self.text, self.rdata):
            s.resolve()
        sections = []
        raw = HEADER_SIZE
        for name, s in ((b".text", self.text), (b".rdata", self.rdata)):
            sections.append((name, s, raw))
            raw += (len(s.buf) + 0xFFF) & ~0xFFF
        image_size = RDATA_VA + ((len(self.rdata.buf) + 0xFFF) & ~0xFFF) - BASE

        out = bytearray(raw)
        out[:4] = b"XBEH"
        cert_off, sec_off, names_off = 0x200, 0x300, 0x500
        entry = self.labels[self.entries[0]]

        def u32(off, v):
            struct.pack_into("<I", out, off, v)

        u32(0x104, BASE)
        u32(0x108, HEADER_SIZE)
        u32(0x10C, image_size)
        u32(0x118, BASE + cert_off)
        u32(0x11C, len(sections))
        u32(0x120, BASE + sec_off)
        u32(0x128, entry ^ RETAIL_ENTRY_KEY)
        u32(0x158, self.labels["kernel_thunk"] ^ RETAIL_THUNK_KEY)
        for i, (name, s, at) in enumerate(sections):
            o = sec_off + i * 56
            u32(o, 7 if s is self.text else 3)  # kWritable | kPreload, and kExecutable for code
            u32(o + 4, s.va)
            u32(o + 8, len(s.buf))
            u32(o + 12, at)
            u32(o + 16, len(s.buf))
            u32(o + 20, BASE + names_off + i * 16)
            out[names_off + i * 16:names_off + i * 16 + len(name)] = name
            out[at:at + len(s.buf)] = s.buf
        return bytes(out)


def config_toml(case, image):
    xbe = {"path": '"default.xbe"', "out": '"generated"', "library": '"Case.dll"', "partitions": "1",
           "code_sections": '[".text"]'}
    xbe.update(case.xbe)
    lines = ["[xbe]"] + [f"{k} = {v(image.labels) if callable(v) else v}" for k, v in xbe.items()]
    lines += ["", "[functions]"] + [f'"{image.labels[e]:#x}" = "{e}"' for e in image.entries]
    if case.toml:
        lines += ["", case.toml(image.labels)]
    return "\n".join(lines) + "\n"


def symbols_h(image):
    lines = ["#pragma once"]
    for name, va in sorted(image.labels.items()):
        lines.append(f"#define VA_{name} 0x{va:X}u")
    for name in image.entries:
        lines.append(f"#define FN_{name} sub_{image.labels[name]:08X}")
    return "\n".join(lines) + "\n"


def main(argv):
    sys.modules["make_xbe"] = sys.modules[__name__]  # cases.py imports this running script, not a second copy of it
    import cases
    if argv[1:] == ["--list"]:
        print("\n".join(cases.CASES))
        return 0
    if len(argv) != 3 or argv[1] not in cases.CASES:
        print(__doc__, file=sys.stderr)
        return 2
    case = cases.CASES[argv[1]]
    image = Image(case.ordinals)
    case.build(image)
    out = Path(argv[2])
    (out / "config").mkdir(parents=True, exist_ok=True)
    data = image.write()
    (out / "default.xbe").write_bytes(data)
    (out / "config" / "game.toml").write_text(config_toml(case, image), newline="\n")
    (out / "case_symbols.h").write_text(symbols_h(image), newline="\n")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
