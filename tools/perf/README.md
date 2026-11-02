# Performance tools

Numbers for whether a change made a game faster or slower.

## From a run's log

`XBE_PERF=1` makes the renderer log three `perf:` lines a second, split into host time, the title thread's waits and the
per frame counts.

```
python tools/perf/perf_report.py logs/game.log -o before.json --start 20 --end 80
python tools/perf/compare.py before.json after.json
```

`perf_report.py` writes p50 and p99 frame time, each second weighted by the frames it drew, with p50 and p99 of every
other number and the machine from the log's system lines. `--start` and `--end` keep a window of the log's clock, to
leave out loading.

`compare.py` prints the two side by side and exits 1 when frame time p50 grew over 5 percent or p99 over 10 percent
(`--p50` and `--p99` change that). It warns when the runs come from different CPUs. It is only as good as its runs: same
machine, same scene, nothing else busy.

## Scenes

`scenes/<game>/<scene>.toml` names a pad script, the frame counting starts at and how many it counts, plus the settings
that make runs comparable. `scenes/example/attract.toml` shows the shape. A game's scenes and its baselines belong in the
game's repository. Nothing here plays a scene yet: a run goes through the harness's `SCRIPT` or a game's own pad hook.

## Microbenchmarks

`bench/` is Google Benchmark over vertex conversion (`gpu/vertex.cpp`), ADPCM decode and the indirect call and kernel call
paths (`cpu/dispatch.cpp`), on synthetic data. The `tests/` build makes `perf_bench`:

```
out/tests/perf_bench/perf_bench
```

The `perf_bench_runs` test only checks it runs. Its timings mean something when built Release on a quiet machine, compared
with the same machine's earlier numbers.
