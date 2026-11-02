#pragma once
// A game's code on Windows is a DLL built with X86_GAME_CODE (cmake/XbeRecomp.cmake). These tests link the generated
// code into the test executable, as the headless harness does on Linux, so on Windows they name the fallback that
// x86_ops.h leaves to the DLL build.
#ifdef _WIN32
#define X86_WEAK_FUNC(name, impl) __pragma(comment(linker, "/alternatename:" #name "=" #impl))
#endif
