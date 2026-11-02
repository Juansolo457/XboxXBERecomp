#!/usr/bin/env python3
"""Compares two perf_report.py summaries; exits 1 when the new one is slower past a threshold.

    compare.py old.json new.json [--p50 5] [--p99 10]

The gate is frame time: p50 may grow 5 percent and p99 10 percent. The rest of the table says where a change went.
"""
import argparse
import json
import sys
from pathlib import Path

# (report group, key) worth a row after the frame time
ROWS = (("host", "host_ms"), ("host", "assemble_ms"), ("host", "bind_ms"), ("host", "present_ms"),
        ("title_thread", "title_record_ms"), ("title_thread", "title_wait_ms"), ("per_frame", "draws"),
        ("per_frame", "readbacks"), ("per_frame", "upload_mb"))


def change(old, new):
    if old is None or new is None:
        return None
    if old == 0:
        return 0.0 if new == 0 else float("inf")
    return (new - old) * 100.0 / old


def compare(old, new, p50_limit, p99_limit):
    """The table's rows, and the failures of the gate."""
    rows, failures = [], []
    for stat, limit in (("p50", p50_limit), ("p99", p99_limit)):
        a, b = old.get("frame_ms", {}).get(stat), new.get("frame_ms", {}).get(stat)
        pct = change(a, b)
        rows.append((f"frame_ms {stat}", a, b, pct))
        if a is None or b is None:
            failures.append(f"frame_ms {stat} missing from a report")
        elif pct > limit:
            failures.append(f"frame_ms {stat} {a:.2f} -> {b:.2f} ({pct:+.1f}%, limit {limit:g}%)")
    for group, key in ROWS:
        a, b = old.get(group, {}).get(key, {}).get("p50"), new.get(group, {}).get(key, {}).get("p50")
        if a is not None or b is not None:
            rows.append((key, a, b, change(a, b)))
    return rows, failures


def number(v):
    return "-" if v is None else f"{v:.2f}"


def main(argv=None):
    p = argparse.ArgumentParser(description="compare two perf_report.py summaries")
    p.add_argument("old", type=Path)
    p.add_argument("new", type=Path)
    p.add_argument("--p50", type=float, default=5.0, help="allowed frame time p50 growth, percent")
    p.add_argument("--p99", type=float, default=10.0, help="allowed frame time p99 growth, percent")
    args = p.parse_args(argv)
    old, new = json.loads(args.old.read_text()), json.loads(args.new.read_text())
    for label, report in (("old", old), ("new", new)):
        s = report.get("system", {})
        print(f"{label}: {s.get('cpu', '?')} / {s.get('d3d12_adapter', '?')}, {report.get('frames', 0)} frames")
    if old.get("system", {}).get("cpu") != new.get("system", {}).get("cpu"):
        print("warning: the two runs come from different CPUs")
    rows, failures = compare(old, new, args.p50, args.p99)
    print(f"{'':20} {'old':>10} {'new':>10} {'change':>9}")
    for name, a, b, pct in rows:
        print(f"{name:20} {number(a):>10} {number(b):>10} {'-' if pct is None else format(pct, '+.1f') + '%':>9}")
    for f in failures:
        print(f"FAIL {f}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
