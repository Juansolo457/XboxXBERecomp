#!/usr/bin/env python3
"""Compares what xbe_recomp wrote for a case against its checked in golden files.

    check_golden.py <generated folder> <golden folder> [--update]

An intended change to the code generator shows here as a diff to read, then --update to accept it.
"""
import difflib
import shutil
import sys
from pathlib import Path

FILES = ("x86_recomp.0.cpp", "x86_func_mapping.cpp", "x86_recomp_shared.h", "image_symbols.h")


def lines(path):
    return path.read_text(encoding="utf-8").replace("\r\n", "\n").splitlines(keepends=True)


def main(argv):
    if len(argv) < 3:
        print(__doc__, file=sys.stderr)
        return 2
    generated, golden = Path(argv[1]), Path(argv[2])
    if "--update" in argv[3:]:
        golden.mkdir(parents=True, exist_ok=True)
        for name in FILES:
            shutil.copyfile(generated / name, golden / name)
        return 0
    bad = 0
    for name in FILES:
        if not (golden / name).exists():
            print(f"{golden / name}: no golden file, read the generated one and run with --update")
            bad += 1
            continue
        diff = list(difflib.unified_diff(lines(golden / name), lines(generated / name), f"golden/{name}", f"generated/{name}"))
        if diff:
            sys.stdout.writelines(diff)
            bad += 1
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
