#!/usr/bin/env python3
"""Turns an XBE_PERF log into a JSON summary.

    perf_report.py <log> [-o out.json] [--start SECONDS] [--end SECONDS]

XBE_PERF=1 makes the renderer write three "perf:" lines a second (LogPerf in runtime/gpu/d3d12.cpp). Each second is one
sample; the report gives p50 and p99 over the samples, with the frame times weighted by the frames each second drew, and
the machine from the system lines the log opens with, so numbers carry where they came from. --start and --end keep the
samples in that window of the log's clock, to skip a loading screen.
"""
import argparse
import json
import re
import sys
from pathlib import Path

NUM = r"(-?[\d.]+)"
LINE = re.compile(r"^\[\s*([\d.]+)\](?: \[\w+\] \[t[0-9a-f]+\])? (.*)$")

# the format strings of LogPerf, with what each number is called in the report
FRAME = re.compile(
    rf"perf: {NUM} fps {NUM} ms/frame \| host {NUM} \(draw {NUM}: assemble {NUM} bind {NUM} \[check {NUM}, upload {NUM}\] "
    rf"compile {NUM}; clear {NUM} copy {NUM} sync {NUM} present {NUM} \[swap {NUM} wait {NUM}\]\) other {NUM}")
FRAME_KEYS = ("fps", "frame_ms", "host_ms", "draw_ms", "assemble_ms", "bind_ms", "check_ms", "upload_ms", "compile_ms",
              "clear_ms", "copy_ms", "sync_ms", "present_ms", "swap_ms", "swap_wait_ms", "other_ms")
TITLE = re.compile(
    rf"perf: title thread per frame: recording {NUM} ms, waiting {NUM} ms \({NUM} waits: read-back {NUM} resource {NUM} "
    rf"fence {NUM} flush {NUM} copy {NUM}, next frame {NUM}\) \| read-backs waited for: {NUM} lock, {NUM} NOOVERWRITE")
TITLE_KEYS = ("title_record_ms", "title_wait_ms", "title_waits", "wait_readback_ms", "wait_resource_ms", "wait_fence_ms",
              "wait_flush_ms", "wait_copy_ms", "wait_next_frame_ms", "readbacks_lock", "readbacks_nooverwrite")
COUNTS = re.compile(
    rf"perf: per frame {NUM} draws \({NUM} raw\) {NUM}k verts {NUM} MB upload {NUM} overflow \| {NUM} checks {NUM} MB "
    rf"hashed \| {NUM} tex uploads {NUM} MB, {NUM} new tex, {NUM} new targets, {NUM} target uploads, {NUM} shrinks, "
    rf"{NUM} read-backs {NUM} late \| {NUM} shaders {NUM} psos this second")
COUNTS_KEYS = ("draws", "raw_draws", "kverts", "upload_mb", "overflow", "checks", "hashed_mb", "tex_uploads", "tex_upload_mb",
               "new_textures", "new_targets", "target_uploads", "shrinks", "readbacks", "readbacks_late", "shaders", "psos")

# the LOGR lines of LogSystem in runtime/app/system_info.cpp and the renderer's adapter line
SYSTEM = (("os", re.compile(r"^OS: (.*)$")), ("cpu", re.compile(r"^CPU: (.*)$")), ("ram", re.compile(r"^RAM: (.*)$")),
          ("desktop", re.compile(r"^Desktop: (.*)$")), ("d3d12_adapter", re.compile(r"^D3D12 adapter: (.*)$")))
GPU = re.compile(r"^GPU \d+: (.*)$")
TITLE_LINE = re.compile(r"^(.*) \((\S+\.xbe)\), (\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})$")


def percentile(values, q):
    """Linear interpolation between closest ranks."""
    s = sorted(values)
    if not s:
        return None
    k = (len(s) - 1) * q / 100.0
    lo = int(k)
    hi = min(lo + 1, len(s) - 1)
    return s[lo] + (s[hi] - s[lo]) * (k - lo)


def weighted_percentile(pairs, q):
    """The value below which q percent of the weight lies: pairs of value and weight."""
    s = sorted(pairs)
    total = sum(w for _, w in s)
    if total <= 0:
        return None
    seen = 0.0
    for value, weight in s:
        seen += weight
        if seen >= total * q / 100.0:
            return value
    return s[-1][0]


def parse(text):
    """The samples and system lines in a log. A sample needs the first perf line; a cut off last second keeps what it has."""
    samples, system = [], {"gpus": []}
    for raw in text.splitlines():
        m = LINE.match(raw.strip())
        time, body = (float(m.group(1)), m.group(2)) if m else (None, raw.strip())
        if (f := FRAME.search(body)) is not None:
            samples.append({"t": time, **dict(zip(FRAME_KEYS, map(float, f.groups())))})
            continue
        if (f := TITLE.search(body)) is not None:
            if samples:
                samples[-1].update(zip(TITLE_KEYS, map(float, f.groups())))
            continue
        if (f := COUNTS.search(body)) is not None:
            if samples:
                samples[-1].update(zip(COUNTS_KEYS, map(float, f.groups())))
            continue
        if (g := GPU.match(body)) is not None:
            system["gpus"].append(g.group(1))
            continue
        for key, pattern in SYSTEM:
            if (f := pattern.match(body)) is not None:
                system[key] = f.group(1)
                break
        else:
            if "title" not in system and (t := TITLE_LINE.match(body)) is not None:
                system.update(title=t.group(1), xbe=t.group(2), date=t.group(3))
    return samples, system


def summarize(samples, system, start=None, end=None):
    kept = [s for s in samples if (start is None or s["t"] is None or s["t"] >= start) and
            (end is None or s["t"] is None or s["t"] <= end)]
    report = {"system": system, "samples": len(kept), "skipped_samples": len(samples) - len(kept)}
    if not kept:
        report["frames"] = 0
        return report
    # a second at 30 fps holds 30 frames of its frame time: the frame percentiles weigh seconds by the frames they drew
    frames = [(s["frame_ms"], s["fps"]) for s in kept]
    report["frames"] = round(sum(s["fps"] for s in kept))
    report["window_s"] = [kept[0]["t"], kept[-1]["t"]]
    report["frame_ms"] = {"p50": weighted_percentile(frames, 50), "p99": weighted_percentile(frames, 99),
                          "max": max(v for v, _ in frames)}
    report["fps"] = {"p50": percentile([s["fps"] for s in kept], 50), "p1": percentile([s["fps"] for s in kept], 1)}
    groups = (("host", [k for k in FRAME_KEYS if k not in ("fps", "frame_ms")]), ("title_thread", TITLE_KEYS),
              ("per_frame", COUNTS_KEYS))
    for name, keys in groups:
        group = {}
        for key in keys:
            values = [s[key] for s in kept if key in s]
            if values:
                group[key] = {"p50": percentile(values, 50), "p99": percentile(values, 99)}
        report[name] = group
    return report


def main(argv=None):
    p = argparse.ArgumentParser(description="XBE_PERF log to JSON")
    p.add_argument("log", type=Path)
    p.add_argument("-o", "--output", type=Path, help="write here instead of stdout")
    p.add_argument("--start", type=float, help="ignore samples before this log time, seconds")
    p.add_argument("--end", type=float, help="ignore samples after this log time, seconds")
    args = p.parse_args(argv)
    samples, system = parse(args.log.read_text(encoding="utf-8", errors="replace"))
    if not samples:
        print(f"{args.log}: no perf lines, run with XBE_PERF=1", file=sys.stderr)
        return 1
    report = summarize(samples, system, args.start, args.end)
    text = json.dumps(report, indent=2, sort_keys=True) + "\n"
    if args.output:
        args.output.write_text(text, newline="\n")
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
