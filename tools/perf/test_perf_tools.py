#!/usr/bin/env python3
"""perf_report.py and compare.py on the sample logs: python test_perf_tools.py"""
import copy
import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import compare  # noqa: E402
import perf_report  # noqa: E402

SAMPLE_DEBUG = HERE / "testdata" / "perf_debug.log"
SAMPLE_RELEASE = HERE / "testdata" / "perf_release.log"


def report_of(path, **kw):
    return perf_report.summarize(*perf_report.parse(path.read_text()), **kw)


class Report(unittest.TestCase):
    def test_samples_and_frame_times(self):
        r = report_of(SAMPLE_DEBUG)
        self.assertEqual(r["samples"], 5)
        self.assertEqual(r["frames"], 60 + 60 + 60 + 30 + 60)
        # 240 of 270 frames took 16.67 ms: the slow second's 33.33 is the p99 and not the p50
        self.assertEqual(r["frame_ms"]["p50"], 16.67)
        self.assertEqual(r["frame_ms"]["p99"], 33.33)
        self.assertEqual(r["frame_ms"]["max"], 33.33)
        self.assertEqual(r["fps"]["p50"], 60.0)

    def test_groups(self):
        r = report_of(SAMPLE_DEBUG)
        self.assertEqual(r["host"]["host_ms"]["p50"], 6.5)
        self.assertEqual(r["host"]["bind_ms"]["p50"], 1.25)
        self.assertEqual(r["host"]["swap_wait_ms"]["p50"], 0.25)
        self.assertEqual(r["title_thread"]["title_record_ms"]["p50"], 3.0)
        self.assertEqual(r["title_thread"]["wait_readback_ms"]["p50"], 0.5)
        self.assertEqual(r["title_thread"]["readbacks_nooverwrite"]["p50"], 0.5)
        self.assertEqual(r["per_frame"]["draws"]["p50"], 412.0)
        self.assertEqual(r["per_frame"]["raw_draws"]["p50"], 400.0)
        self.assertEqual(r["per_frame"]["readbacks"]["p50"], 1.5)
        self.assertEqual(r["per_frame"]["psos"]["p50"], 2.0)

    def test_system_lines_carry_the_machine(self):
        s = report_of(SAMPLE_DEBUG)["system"]
        self.assertEqual(s["cpu"], "Test CPU 4000 (16 threads)")
        self.assertEqual(s["os"], "Windows 10.0 build 26100")
        self.assertEqual(s["ram"], "32.0 GB")
        self.assertEqual(s["gpus"], ["Test GPU 100 (primary)", "Other GPU"])
        self.assertEqual(s["d3d12_adapter"], "Test GPU 100")
        self.assertEqual(s["desktop"], "1920x1080")
        self.assertEqual(s["xbe"], "default.xbe")
        self.assertEqual(s["title"], "Sample Title")
        self.assertEqual(s["date"], "2026-11-01 12:00:00")

    def test_release_log_format(self):
        # a release build has no level and thread columns
        r = report_of(SAMPLE_RELEASE)
        self.assertEqual(r["samples"], 2)
        self.assertEqual(r["frame_ms"]["p50"], 8.33)
        self.assertEqual(r["system"]["cpu"], "Test CPU 4000 (16 threads)")

    def test_window(self):
        r = report_of(SAMPLE_DEBUG, start=2.5, end=4.5)
        self.assertEqual(r["samples"], 2)
        self.assertEqual(r["skipped_samples"], 3)
        self.assertEqual(r["window_s"], [3.001, 4.001])

    def test_empty_window(self):
        r = report_of(SAMPLE_DEBUG, start=100)
        self.assertEqual(r["samples"], 0)
        self.assertEqual(r["frames"], 0)
        self.assertNotIn("frame_ms", r)

    def test_cut_off_last_second_keeps_what_it_has(self):
        text = SAMPLE_DEBUG.read_text()
        cut = text[:text.rindex("perf: title thread")].rsplit("\n", 1)[0]
        samples, system = perf_report.parse(cut)
        r = perf_report.summarize(samples, system)
        self.assertEqual(r["samples"], 5)
        self.assertEqual(r["per_frame"]["draws"]["p50"], 412.0)

    def test_every_format_string_of_the_renderer_parses(self):
        # the pattern and the source must not drift apart
        source = (HERE.parent.parent / "runtime" / "gpu" / "d3d12.cpp").read_text(encoding="utf-8")
        for phrase in ("ms/frame | host", "title thread per frame: recording", "per frame %.0f draws"):
            self.assertIn(phrase, source)

    def test_cli_writes_json(self):
        with tempfile.TemporaryDirectory() as d:
            out = Path(d) / "r.json"
            self.assertEqual(perf_report.main([str(SAMPLE_DEBUG), "-o", str(out)]), 0)
            self.assertEqual(json.loads(out.read_text())["samples"], 5)

    def test_cli_refuses_a_log_with_no_perf_lines(self):
        with tempfile.TemporaryDirectory() as d:
            log = Path(d) / "empty.log"
            log.write_text("[    0.100] [info] [t0001] nothing here\n")
            with redirect_stderr(io.StringIO()):
                self.assertEqual(perf_report.main([str(log)]), 1)


class Percentile(unittest.TestCase):
    def test_interpolates(self):
        self.assertEqual(perf_report.percentile([1, 2, 3, 4, 5], 50), 3)
        self.assertEqual(perf_report.percentile([10, 20], 50), 15)
        self.assertEqual(perf_report.percentile([7], 99), 7)
        self.assertIsNone(perf_report.percentile([], 50))

    def test_weights(self):
        self.assertEqual(perf_report.weighted_percentile([(1, 99), (100, 1)], 50), 1)
        self.assertEqual(perf_report.weighted_percentile([(1, 98), (100, 2)], 99), 100)
        self.assertIsNone(perf_report.weighted_percentile([], 50))


class Compare(unittest.TestCase):
    def setUp(self):
        self.base = report_of(SAMPLE_DEBUG)

    def slower(self, p50=1.0, p99=1.0):
        new = copy.deepcopy(self.base)
        new["frame_ms"]["p50"] *= p50
        new["frame_ms"]["p99"] *= p99
        return new

    def test_same_passes(self):
        self.assertEqual(compare.compare(self.base, self.base, 5, 10)[1], [])

    def test_thresholds(self):
        self.assertEqual(compare.compare(self.base, self.slower(p50=1.049), 5, 10)[1], [])
        self.assertEqual(len(compare.compare(self.base, self.slower(p50=1.051), 5, 10)[1]), 1)
        self.assertEqual(compare.compare(self.base, self.slower(p99=1.099), 5, 10)[1], [])
        self.assertEqual(len(compare.compare(self.base, self.slower(p99=1.101), 5, 10)[1]), 1)
        self.assertEqual(len(compare.compare(self.base, self.slower(1.5, 1.5), 5, 10)[1]), 2)

    def test_faster_passes(self):
        self.assertEqual(compare.compare(self.base, self.slower(0.5, 0.5), 5, 10)[1], [])

    def test_missing_is_a_failure(self):
        self.assertEqual(len(compare.compare(self.base, {}, 5, 10)[1]), 2)

    def test_cli_exit_codes(self):
        with tempfile.TemporaryDirectory() as d:
            old, ok, bad = Path(d) / "old.json", Path(d) / "ok.json", Path(d) / "bad.json"
            old.write_text(json.dumps(self.base))
            ok.write_text(json.dumps(self.slower(1.01, 1.01)))
            bad.write_text(json.dumps(self.slower(1.2, 1.2)))
            with redirect_stdout(io.StringIO()) as out:
                self.assertEqual(compare.main([str(old), str(ok)]), 0)
                self.assertEqual(compare.main([str(old), str(bad)]), 1)
            self.assertIn("FAIL frame_ms p50", out.getvalue())
            self.assertIn("+20.0%", out.getvalue())


class Scenes(unittest.TestCase):
    def test_example_scene_names_what_a_run_needs(self):
        import tomllib
        scene = tomllib.loads((HERE / "scenes" / "example" / "attract.toml").read_text())
        for key in ("pad_script", "start_frame", "frames"):
            self.assertIn(key, scene)


if __name__ == "__main__":
    unittest.main()
