// Microbenchmarks of the hot paths XBE_PERF names: vertex conversion, ADPCM decode, indirect calls. No game: synthetic
// data on the real guest address space.
#include <benchmark/benchmark.h>

// the decoder is private to its source, so this file is its translation unit
#include "xdk/dsound.cpp"

#include <memory>
#include <random>

#include "gpu/vertex.h"

namespace kernel {
uint8_t* g_base = nullptr;
const xbe::Image* g_image = nullptr;
}  // namespace kernel

namespace audio {
void StartOutput(std::function<void(float*, uint32_t)>) {}
}  // namespace audio

#ifndef _WIN32
extern "C" const X86FuncMapping X86FuncMappings[] = {{0, nullptr}};
extern "C" const uint32_t X86Continuations[] = {0};
extern "C" const uint32_t X86CodeBase = 0, X86CodeEnd = 0, X86EntryPoint = 0;
#endif

namespace dispatch {
uint32_t HostThunk(X86Func* fn);
}

namespace {

// a state drawing count vertices of the FVF from one guest buffer of random bytes
struct VertexDraw {
    std::unique_ptr<gpu::State> s = std::make_unique<gpu::State>();
    gpu::Assembled out;
    VertexDraw(uint32_t fvf, uint32_t stride, uint32_t count) {
        uint32_t header = mem::AllocSystem(32);
        uint32_t data = mem::AllocSystem(stride * count);
        std::minstd_rand random(1);
        for (uint32_t i = 0; i < stride * count; i++) kernel::G<uint8_t>(data + i) = uint8_t(random());
        kernel::W32(header + 4, data);
        s->streams[0] = {header, stride};
        s->vs_handle = fvf;
    }
};

// floats through unchanged: position, normal, one texture set
void BM_AssembleFloats(benchmark::State& state) {
    const uint32_t count = uint32_t(state.range(0));
    VertexDraw d(0x2 | 0x10 | 0x100, 32, count);
    for (auto _ : state) {
        benchmark::DoNotOptimize(gpu::Assemble(*d.s, 5, 0, count, nullptr, d.out));
        benchmark::DoNotOptimize(d.out.vertices.data());
    }
    state.SetItemsProcessed(state.iterations() * count);
}
BENCHMARK(BM_AssembleFloats)->Arg(1000)->Arg(30000);

// the types that convert: D3DCOLOR, NORMPACKED3 and NORMSHORT from one declaration
void BM_AssemblePacked(benchmark::State& state) {
    const uint32_t count = uint32_t(state.range(0));
    VertexDraw d(0, 16, count);
    d.s->vs = std::make_shared<gpu::VertexShader>();
    d.s->vs->inputs[0] = {0, 0, 0x41};
    d.s->vs->inputs[2] = {0, 8, 0x16};
    d.s->vs->inputs[3] = {0, 12, 0x40};
    d.s->vs_handle = 1;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gpu::Assemble(*d.s, 5, 0, count, nullptr, d.out));
        benchmark::DoNotOptimize(d.out.vertices.data());
    }
    state.SetItemsProcessed(state.iterations() * count);
}
BENCHMARK(BM_AssemblePacked)->Arg(1000)->Arg(30000);

void BM_AdpcmBlock(benchmark::State& state) {
    const int channels = int(state.range(0));
    std::vector<uint8_t> block(size_t(36 * channels));
    std::minstd_rand random(1);
    for (uint8_t& b : block) b = uint8_t(random());
    for (int c = 0; c < channels; c++) block[size_t(c * 4 + 2)] = 20;  // a sane step index
    int16_t out[65 * 8];
    for (auto _ : state) {
        DecodeBlock(block.data(), channels, out);
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(state.iterations() * int64_t(block.size()));
}
BENCHMARK(BM_AdpcmBlock)->Arg(1)->Arg(2);

void HostFunction(X86Context& ctx, uint8_t*) { ctx.eax++; }

// x86_icall through a host thunk, the path a call through a pointer takes after the code table's check
void BM_IndirectCall(benchmark::State& state) {
    uint32_t target = dispatch::HostThunk(HostFunction);
    X86Context ctx{};
    for (auto _ : state) x86_icall(ctx, kernel::g_base, target);
    benchmark::DoNotOptimize(ctx.eax);
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_IndirectCall);

void BM_KernelCall(benchmark::State& state) {
    X86Context ctx{};
    ctx.esp = mem::AllocStack(0x10000) - 64;
    const uint32_t esp = ctx.esp;
    constexpr uint32_t kNtQueryDirectoryFile = 207;
    for (auto _ : state) {
        ctx.esp = esp;
        kernel::W32(ctx.esp + 4, 0);  // no such handle: the call turns back at once
        x86_kernel(ctx, kernel::g_base, kNtQueryDirectoryFile);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_KernelCall);

}  // namespace

int main(int argc, char** argv) {
    if (!mem::Init()) return 1;
    kernel::g_base = mem::Base();
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
