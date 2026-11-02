# Tests

The SDK's own tests. They need no game: no XBE, no disc, no generated code from a title. Everything they run on is
written here or in the SDK.

```
cmake -S tests -B out/tests -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build out/tests
ctest --test-dir out/tests --output-on-failure
```

Windows with Visual Studio's Clang, or Linux with clang; Python 3 and Ninja on both. `.github/workflows/tests.yml` runs
exactly this on `windows-latest` and `ubuntu-latest`.

## Recompiler: `recomp/`

Each case in `cases.py` is hand assembled x86, one thing the analysis or the code generator handles. `make_xbe.py` writes
it into a minimal XBE with a config, `xbe_recomp` turns that into C++, Clang compiles it with `cases/<name>.cpp`, and two
tests run:

- `recomp_<name>` calls the recompiled functions on a guest memory and checks registers and memory. `guest.cpp` stands in
  for the runtime services the generated code calls.
- `recomp_golden_<name>` compares the generated C++ with `golden/<name>/`. Its comment lines show the disassembly, so a
  golden file also says what the bytes in `cases.py` decode to. A code generator change that was meant shows as a diff to
  read, then to accept:

  ```
  python tests/recomp/check_golden.py out/tests/recomp/<name>/generated tests/recomp/golden/<name> --update
  ```

To add a case: a function in `cases.py` with the `@case()` decorator, its checks in `cases/<name>.cpp`, then the golden
files as above. `t.fn("name")` declares a function the checks call as `FN_name`; any label is `VA_name`.

## Runtime: `runtime/`

`runtime_tests` links the kernel, DirectSound and vertex sources as a game does, leaving out the window, GPU backend,
audio output and `kernel/boot.cpp`, on the real 4 GB guest address space:

- `test_objects.cpp` waits and wake ups: which waits sit on which objects, one release per set, timeouts, mutants across
  threads, guest dispatcher headers, timers, APCs and DPCs
- `test_memory.cpp` the virtual, contiguous, system and stack allocators, and the physical memory's two windows
- `test_files.cpp` `NtQueryDirectoryFile` masks and renames, through the exports with the guest's calling convention
- `test_dsound.cpp` is `xdk/dsound.cpp`'s translation unit, to reach its private decoder and cursors: ADPCM against hand
  worked values and the format's encoder, the play cursor against a frame by frame model
- `test_vertex.cpp` each vertex type's conversion and each primitive's index list

`shader_corpus` runs the shader translator on inputs written out in `shader_corpus.cpp`, none from a game.
`shader_corpus_golden` compares its HLSL with `golden/`, `check_shaders.py golden <out> tests/runtime/golden --update`
accepts a change. `shader_corpus_compile` puts every file through each compiler it finds: fxc at the renderer's profiles,
dxc at shader model 6. The HLSL is checked to compile, not run. With neither compiler the test reports as skipped.

## Known bugs

A test marked `doctest::should_fail()` states the right behaviour of something the SDK gets wrong today. The suite stays
green and goes red when the bug is fixed, which is the signal to remove the marker.
