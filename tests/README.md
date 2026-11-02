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

## Known bugs

A test marked `doctest::should_fail()` states the right behaviour of something the SDK gets wrong today. The suite stays
green and goes red when the bug is fixed, which is the signal to remove the marker.
