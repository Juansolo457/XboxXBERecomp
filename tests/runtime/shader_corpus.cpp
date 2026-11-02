// Writes the shader translator's output for a fixed set of inputs: <name>.<stage>.hlsl in the folder given. The inputs
// are written out here, none comes from a game: vertex programs through the microcode's field layout, pixel states
// through the XDK's register combiner encoding.
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "gpu/shaders.h"

using namespace gpu;
namespace fs = std::filesystem;

namespace {

constexpr uint32_t kXyzw = 0x1B;  // the identity swizzle: x y z w, two bits each
enum Mux : uint32_t { kTemp = 1, kInput = 2, kConst = 3 };
enum Mac : uint32_t { kMov = 1, kMul, kAdd, kMad, kDp3, kDph, kDp4, kDst, kMin, kMax, kSlt, kSge, kArl };
enum Ilu : uint32_t { kIMov = 1, kRcp, kRcc, kRsq, kExp, kLog, kLit };
enum Output : uint32_t { kOPos = 0, kOD0 = 3, kOD1 = 4, kOFog = 5, kOPts = 6, kOB0 = 7, kOB1 = 8, kOT0 = 9, kOT1 = 10 };

struct Operand {
    uint32_t mux = kTemp, reg = 0, swizzle = kXyzw, neg = 0;
};
Operand V(uint32_t, uint32_t swizzle = kXyzw) { return {kInput, 0, swizzle, 0}; }
Operand C(uint32_t swizzle = kXyzw) { return {kConst, 0, swizzle, 0}; }
Operand R(uint32_t r, uint32_t swizzle = kXyzw) { return {kTemp, r, swizzle, 0}; }

struct Ins {
    uint32_t mac = 0, ilu = 0;
    Operand a, b, c;
    uint32_t input = 0, constant = 0;  // the v and c register this instruction reads, shared by its operands
    uint32_t mac_mask = 0, ilu_mask = 0, temp = 7;  // temp 7 with no mask writes nothing
    uint32_t out_mask = 0, out = 0, out_ilu = 0, relative = 0;
};

// one instruction's four dwords, the last one flagged
void Encode(const Ins& i, bool last, std::vector<uint32_t>& p) {
    uint32_t w1 = i.mac << 21 | i.ilu << 25 | i.constant << 13 | i.input << 9 | i.a.neg << 8 | i.a.swizzle;
    uint32_t w2 = i.a.reg << 28 | i.a.mux << 26 | i.b.neg << 25 | i.b.swizzle << 17 | i.b.reg << 13 | i.b.mux << 11 |
                  i.c.neg << 10 | i.c.swizzle << 2 | (i.c.reg >> 2);
    uint32_t w3 = (i.c.reg & 3) << 30 | i.c.mux << 28 | i.mac_mask << 24 | i.temp << 20 | i.ilu_mask << 16 | i.out_mask << 12 |
                  (i.out_mask ? 1u << 11 : 0) | i.out << 3 | i.out_ilu << 2 | i.relative << 1 | (last ? 1 : 0);
    p.insert(p.end(), {0u, w1, w2, w3});
}

std::vector<uint32_t> Program(const std::vector<Ins>& ins) {
    std::vector<uint32_t> p;
    for (size_t i = 0; i < ins.size(); i++) Encode(ins[i], i + 1 == ins.size(), p);
    return p;
}

Ins MovOut(uint32_t input, uint32_t out) {
    Ins i;
    i.mac = kMov;
    i.a = V(input);
    i.input = input;
    i.out = out;
    i.out_mask = 0xF;
    return i;
}

fs::path g_dir;
int g_count = 0;

void Write(const std::string& name, const char* stage, const std::string& hlsl) {
    std::ofstream f(g_dir / (name + "." + stage + ".hlsl"), std::ios::binary);
    f << hlsl;
    g_count++;
}

void VertexPrograms() {
    constexpr uint16_t kPosColorTex = 1u << 0 | 1u << 3 | 1u << 9;
    Write("vp_passthrough", "vs", VertexProgramHlsl(Program({MovOut(0, kOPos), MovOut(3, kOD0), MovOut(9, kOT0)}), kPosColorTex));

    // the usual transform: a dp4 per lane from four constants
    std::vector<Ins> transform;
    for (uint32_t lane = 0; lane < 4; lane++) {
        Ins dp;
        dp.mac = kDp4;
        dp.a = V(0);
        dp.b = C();
        dp.input = 0;
        dp.constant = 96 + lane;
        dp.out = kOPos;
        dp.out_mask = 8u >> lane;
        transform.push_back(dp);
    }
    transform.push_back(MovOut(3, kOD0));
    Write("vp_transform", "vs", VertexProgramHlsl(Program(transform), kPosColorTex));

    // every MAC opcode, into temporaries, then position from one
    std::vector<Ins> macs;
    for (uint32_t op : {kMov, kMul, kAdd, kMad, kDp3, kDph, kDp4, kDst, kMin, kMax, kSlt, kSge}) {
        Ins m;
        m.mac = op;
        m.a = V(0);
        m.b = R(0);
        m.c = C();
        m.constant = 100;
        m.temp = op % 11;
        m.mac_mask = 0xF;
        macs.push_back(m);
    }
    Ins last;
    last.mac = kMov;
    last.a = R(1);
    last.out = kOPos;
    last.out_mask = 0xF;
    macs.push_back(last);
    Write("vp_mac_ops", "vs", VertexProgramHlsl(Program(macs), kPosColorTex));

    // every ILU opcode, each read through operand C's x
    std::vector<Ins> ilus;
    for (uint32_t op : {kIMov, kRcp, kRcc, kRsq, kExp, kLog, kLit}) {
        Ins l;
        l.ilu = op;
        l.c = V(0);
        l.c.swizzle = 0;
        l.temp = op;
        l.ilu_mask = 0xF;
        ilus.push_back(l);
    }
    ilus.push_back(MovOut(0, kOPos));
    Write("vp_ilu_ops", "vs", VertexProgramHlsl(Program(ilus), kPosColorTex));

    // MAC and ILU paired, the ILU to R1, the MAC to the output
    Ins pair;
    pair.mac = kMul;
    pair.ilu = kRsq;
    pair.a = V(0);
    pair.b = C();
    pair.c = R(2);
    pair.constant = 101;
    pair.mac_mask = 0xF;
    pair.ilu_mask = 8;
    pair.temp = 3;
    pair.out = kOPos;
    pair.out_mask = 0xF;
    Write("vp_paired", "vs", VertexProgramHlsl(Program({pair}), kPosColorTex));

    // arl and a constant read through a0
    Ins arl;
    arl.mac = kArl;
    arl.a = V(3, 0);
    arl.input = 3;
    Ins read;
    read.mac = kMov;
    read.a = C();
    read.constant = 110;
    read.relative = 1;
    read.out = kOD0;
    read.out_mask = 0xF;
    Write("vp_relative", "vs", VertexProgramHlsl(Program({arl, read, MovOut(0, kOPos)}), kPosColorTex));

    // R12 reads and writes oPos; fog and point size take their lanes; a missing input reads its default
    Ins r12;
    r12.mac = kMov;
    r12.a = V(0);
    r12.temp = 12;
    r12.mac_mask = 0xF;
    Ins fog;
    fog.mac = kMov;
    fog.a = R(12);
    fog.out = kOFog;
    fog.out_mask = 8;
    Ins pts;
    pts.mac = kMov;
    pts.a = R(12);
    pts.out = kOPts;
    pts.out_mask = 8;
    Ins missing = MovOut(3, kOB0);
    Ins back = MovOut(4, kOB1);
    Write("vp_outputs", "vs", VertexProgramHlsl(Program({r12, fog, pts, missing, back}), 1u << 0));

    // negated operands and a swizzle, and the elements the input assembler hands over raw
    Ins neg = MovOut(9, kOT1);
    neg.a.neg = 1;
    neg.a.swizzle = 0xE4;
    uint64_t decode = uint64_t(1) << 0 | uint64_t(4) << (9 * 4);  // v0 w = 1, v9 packed
    Write("vp_decode", "vs", VertexProgramHlsl(Program({MovOut(0, kOPos), neg}), kPosColorTex, decode));
}

void Fixed() {
    constexpr uint16_t kPosNormalColorTex = 1u << 0 | 1u << 2 | 1u << 3 | 1u << 9;
    Write("ff_unlit", "vs", FixedVertexHlsl(1u << 0 | 1u << 3 | 1u << 9, false));
    Write("ff_pretransformed", "vs", FixedVertexHlsl(1u << 0 | 1u << 3 | 1u << 9, true));
    FixedKey lit;
    lit.light = 1 | 2 | 8;
    lit.types = 3 | 1 << 2 | 2 << 4;  // directional, point, spot
    lit.tex[0] = 0 | 2 << 8 | 2 << 12;
    Write("ff_lit", "vs", FixedVertexHlsl(kPosNormalColorTex, false, 0, lit));
    FixedKey gen;
    gen.tex[0] = 1 << 4 | 3 << 8 | 0x800 | 3 << 12;  // reflection vectors, projected
    Write("ff_texgen", "vs", FixedVertexHlsl(kPosNormalColorTex, false, 0, gen));
}

constexpr uint32_t kTop_Modulate = 4;
constexpr uint32_t kTa_Diffuse = 0;
constexpr uint32_t kTa_Texture = 2;

PixelKey FixedKeyStage() {
    PixelKey k{};
    k.tex[0] = {kTex2D, 0, 0, 0};
    // stage 0: colorop, arg1, arg2, arg0, alphaop, arg1, arg2, arg0, result
    const uint32_t stage[9] = {kTop_Modulate, kTa_Texture, kTa_Diffuse, 0, kTop_Modulate, kTa_Texture, kTa_Diffuse, 0, 1};
    std::memcpy(k.ff[0], stage, sizeof(stage));
    k.ff[1][0] = 1;  // D3DTOP_DISABLE
    k.ff[1][4] = 1;
    return k;
}

// t0 * v0 into r0, r0 out through the final combiner
PixelKey CombinerKey() {
    PixelKey k{};
    k.programmable = 1;
    k.count = 1;
    k.tex[0] = {kTex2D, 0, 0, 0};
    k.tex_modes = 1;
    k.rgb_in[0] = 0x08040000;   // A t0, B v0, C and D zero
    k.a_in[0] = 0x18140000;     // the same, alpha
    k.rgb_out[0] = 0x000000C0;  // AB to r0
    k.a_out[0] = 0x000000C0;
    k.final_abcd = 0x0000000C;  // D r0
    k.final_efg = 0x00001C00;   // G r0 alpha
    return k;
}

void Pixels() {
    Write("ps_ff_modulate", "ps", PixelHlsl(FixedKeyStage()));

    PixelKey ats = FixedKeyStage();
    ats.alphatest = 1;
    Write("ps_ff_alphatest", "ps", PixelHlsl(ats));

    PixelKey sided = FixedKeyStage();
    sided.two_sided = 1;
    sided.flat = 1;
    Write("ps_ff_two_sided", "ps", PixelHlsl(sided));

    Write("ps_combiner", "ps", PixelHlsl(CombinerKey()));

    PixelKey kill = CombinerKey();
    kill.alphakill = 1;
    Write("ps_combiner_kill", "ps", PixelHlsl(kill));

    PixelKey cube = CombinerKey();
    cube.tex[0].kind = kTexCube;
    cube.tex_modes = 3;
    Write("ps_cube", "ps", PixelHlsl(cube));

    PixelKey shadow = CombinerKey();
    shadow.tex[0] = {kTex2D, 0, 1, 0};
    Write("ps_shadow", "ps", PixelHlsl(shadow));

    PixelKey volume = CombinerKey();
    volume.tex[0].kind = kTex3D;
    volume.tex_modes = 2;
    Write("ps_volume", "ps", PixelHlsl(volume));

    PixelKey bump = CombinerKey();  // t1 fetched at t0's bump map
    bump.tex[1] = {kTex2D, 0, 0, 0};
    bump.tex_modes = 1 | 6 << 5;
    Write("ps_bumpenv", "ps", PixelHlsl(bump));
}

void Others() {
    std::string vs = FixedVertexHlsl(1u << 0 | 1u << 9, true);
    Write("quad", "vs", QuadHlsl(vs));
    Write("reversed_depth", "vs", ReversedDepthHlsl(FixedVertexHlsl(1u << 0, false)));
    Write("points", "gs", PointHlsl());
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: shader_corpus <out folder>\n");
        return 2;
    }
    g_dir = argv[1];
    fs::create_directories(g_dir);
    VertexPrograms();
    Fixed();
    Pixels();
    Others();
    std::printf("%d shaders written to %s\n", g_count, argv[1]);
    return 0;
}
