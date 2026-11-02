#!/usr/bin/env python3
"""The shader translator's output against golden files, and through the compilers.

    check_shaders.py golden <out folder> <golden folder> [--update]
    check_shaders.py compile <out folder>

compile runs every compiler it finds: fxc at the profiles the renderer's D3DCompile uses, dxc at shader model 6. It exits
77, which CTest reports as skipped, when there is none. Goldens show a change, the compilers show it still builds.
"""
import difflib
import glob
import os
import shutil
import subprocess
import sys
from pathlib import Path


def lines(path):
    return path.read_text(encoding="utf-8").replace("\r\n", "\n").splitlines(keepends=True)


def golden(out, gold, update):
    names = sorted(p.name for p in out.glob("*.hlsl"))
    if update:
        gold.mkdir(parents=True, exist_ok=True)
        for old in gold.glob("*.hlsl"):
            old.unlink()
        for name in names:
            shutil.copyfile(out / name, gold / name)
        return 0
    bad = 0
    for name in sorted({p.name for p in gold.glob("*.hlsl")} | set(names)):
        if not (gold / name).exists() or not (out / name).exists():
            print(f"{name}: only in one of the golden and generated folders")
            bad += 1
            continue
        diff = list(difflib.unified_diff(lines(gold / name), lines(out / name), f"golden/{name}", f"generated/{name}"))
        if diff:
            sys.stdout.writelines(diff)
            bad += 1
    return 1 if bad else 0


def find(tool):
    found = shutil.which(tool)
    if found:
        return found
    kits = os.environ.get("ProgramFiles(x86)")
    if kits:
        hits = sorted(glob.glob(f"{kits}/Windows Kits/10/bin/*/x64/{tool}.exe"))
        if hits:
            return hits[-1]
    return None


def compile_all(out):
    fxc, dxc = find("fxc"), find("dxc")
    if not fxc and not dxc:
        print("no fxc or dxc found")
        return 77
    print(f"fxc: {fxc}\ndxc: {dxc}")
    failed = 0
    for path in sorted(out.glob("*.hlsl")):
        stage = path.name.split(".")[-2]
        runs = []
        if fxc:
            runs.append([fxc, "/nologo", "/T", f"{stage}_5_0", "/E", "main", "/O1", "/Fo", os.devnull, str(path)])
        if dxc:
            # the language version fxc takes: 2021 turned vector ?: and && into errors
            runs.append([dxc, "-HV", "2018", "-T", f"{stage}_6_0", "-E", "main", "-O1", "-Fo", str(path) + ".dxil", str(path)])
        for run in runs:
            r = subprocess.run(run, capture_output=True, text=True)
            if r.returncode:
                failed += 1
                print(f"--- {path.name}: {Path(run[0]).name} failed\n{r.stdout}{r.stderr}")
    print(f"{failed} failures")
    return 1 if failed else 0


def main(argv):
    if len(argv) >= 4 and argv[1] == "golden":
        return golden(Path(argv[2]), Path(argv[3]), "--update" in argv[4:])
    if len(argv) == 3 and argv[1] == "compile":
        return compile_all(Path(argv[2]))
    print(__doc__, file=sys.stderr)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
