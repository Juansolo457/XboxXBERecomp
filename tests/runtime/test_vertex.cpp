#include <dxgiformat.h>

#include <cstring>
#include <memory>
#include <vector>

#include "gpu/vertex.h"
#include "support.h"

using namespace gpu;

namespace {

constexpr float kEps = 1e-5f;

// a state with one guest vertex buffer holding bytes
struct Draw {
    std::unique_ptr<State> s = std::make_unique<State>();
    Assembled out;
    uint32_t header = mem::AllocSystem(32);
    uint32_t data = 0;

    template <typename T>
    void Buffer(const std::vector<T>& v, uint32_t stride) {
        data = mem::AllocSystem(uint32_t(v.size() * sizeof(T)));
        std::memcpy(kernel::HostPtr(data), v.data(), v.size() * sizeof(T));
        kernel::W32(header + 4, data);
        s->streams[0] = {header, stride};
    }
    // a programmable draw reading one register from the buffer
    void Declare(int reg, uint32_t type, uint32_t offset = 0) {
        s->vs = std::make_shared<VertexShader>();
        s->vs->inputs[size_t(reg)] = {0, offset, type};
        s->vs_handle = 1;
    }
    ~Draw() {
        mem::FreeSystem(header);
        if (data) mem::FreeSystem(data);
    }
    const float* Attr(uint32_t vertex, uint32_t reg_slot = 0, uint32_t nregs = 1) const {
        return &out.vertices[(vertex * nregs + reg_slot) * 4];
    }
};

void CheckAttr(const float* v, float x, float y, float z, float w) {
    CHECK(v[0] == doctest::Approx(x).epsilon(kEps));
    CHECK(v[1] == doctest::Approx(y).epsilon(kEps));
    CHECK(v[2] == doctest::Approx(z).epsilon(kEps));
    CHECK(v[3] == doctest::Approx(w).epsilon(kEps));
}

using Indices = std::vector<uint32_t>;

}  // namespace

TEST_CASE("an FVF names its registers and offsets") {
    VertexShader vs;
    FvfInputs(0x2 | 0x10 | 0x40 | 0x100, vs);  // position, normal, diffuse, one texture set
    CHECK(vs.inputs[0].offset == 0);
    CHECK(vs.inputs[0].type == 0x32);
    CHECK(vs.inputs[2].offset == 12);
    CHECK(vs.inputs[3].offset == 24);
    CHECK(vs.inputs[3].type == 0x40);
    CHECK(vs.inputs[9].offset == 28);
    CHECK(vs.inputs[9].type == 0x22);
    CHECK(vs.stream_stride[0] == 36);
    CHECK(vs.inputs[1].stream == -1);
}

TEST_CASE("each vertex type converts to floats") {
    Draw d;
    SUBCASE("D3DCOLOR is bytes B G R A") {
        d.Buffer<uint8_t>({51, 102, 153, 255}, 4);
        d.Declare(3, 0x40);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 0.6f, 0.4f, 0.2f, 1.0f);
    }
    SUBCASE("NORMSHORT scales and keeps -1") {
        d.Buffer<int16_t>({32767, -32768, 0, 16384}, 8);
        d.Declare(0, 0x41);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 1.0f, -1.0f, 0.0f, 16384 / 32767.0f);
    }
    SUBCASE("FLOAT leaves missing lanes at 0 0 0 1") {
        d.Buffer<float>({2.5f, -3.5f}, 8);
        d.Declare(0, 0x22);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 2.5f, -3.5f, 0.0f, 1.0f);
    }
    SUBCASE("PBYTE scales to one") {
        d.Buffer<uint8_t>({255, 0, 51, 0}, 4);
        d.Declare(0, 0x34);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 1.0f, 0.0f, 0.2f, 1.0f);
    }
    SUBCASE("SHORT stays integers") {
        d.Buffer<int16_t>({-5, 7}, 4);
        d.Declare(0, 0x25);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), -5.0f, 7.0f, 0.0f, 1.0f);
    }
    SUBCASE("NORMPACKED3 is 11 11 10 signed") {
        uint32_t packed = 1023u | (uint32_t(-1023 & 0x7FF) << 11) | (uint32_t(511) << 22);
        d.Buffer<uint32_t>({packed}, 4);
        d.Declare(0, 0x16);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 1.0f, -1.0f, 1.0f, 1.0f);
    }
    SUBCASE("FLOAT2H is x y w") {
        d.Buffer<float>({1.0f, 2.0f, 3.0f}, 12);
        d.Declare(0, 0x72);
        REQUIRE(Assemble(*d.s, 1, 0, 1, nullptr, d.out));
        CheckAttr(d.Attr(0), 1.0f, 2.0f, 0.0f, 3.0f);
    }
}

TEST_CASE("primitives become index lists") {
    Draw d;
    std::vector<float> positions(3 * 8, 0.0f);
    d.Buffer(positions, 12);
    d.s->vs_handle = 0x2;  // FVF position
    auto indices = [&](uint32_t prim, uint32_t count) {
        REQUIRE(Assemble(*d.s, prim, 0, count, nullptr, d.out));
        return d.out.indices;
    };
    CHECK(indices(1, 2) == Indices{0, 1});
    CHECK(d.out.topology == Topology::Points);
    CHECK(indices(2, 4) == Indices{0, 1, 2, 3});
    CHECK(d.out.topology == Topology::Lines);
    CHECK(indices(4, 3) == Indices{0, 1, 1, 2});
    CHECK(indices(3, 3) == Indices{0, 1, 1, 2, 2, 0});  // a loop closes
    CHECK(indices(5, 3) == Indices{0, 1, 2});
    CHECK(d.out.topology == Topology::Triangles);
    CHECK(indices(6, 4) == Indices{0, 1, 2, 2, 1, 3});  // odd strip triangles swap to keep their winding
    CHECK(indices(7, 4) == Indices{0, 1, 2, 0, 2, 3});
    CHECK(indices(10, 4) == Indices{0, 1, 2, 0, 2, 3});
    CHECK(indices(8, 4) == Indices{0, 1, 2, 0, 2, 3});
    CHECK(indices(9, 6) == Indices{0, 1, 2, 1, 3, 2, 2, 3, 4, 3, 5, 4});
    CHECK_FALSE(Assemble(*d.s, 0, 0, 3, nullptr, d.out));
    CHECK_FALSE(Assemble(*d.s, 11, 0, 3, nullptr, d.out));
    CHECK_FALSE(Assemble(*d.s, 5, 0, 0, nullptr, d.out));
}

TEST_CASE("an indexed draw reads the vertices its indices name, with the base vertex") {
    Draw d;
    std::vector<float> values;
    for (int i = 0; i < 8; i++) values.push_back(float(i));
    d.Buffer(values, 4);
    d.Declare(0, 0x12);
    d.s->base_vertex = 2;
    const uint16_t ix[] = {1, 3, 2};
    REQUIRE(Assemble(*d.s, 5, 0, 3, reinterpret_cast<const uint8_t*>(ix), d.out));
    CHECK(d.out.vertex_count == 3);  // the range 1 to 3
    CHECK(d.Attr(0)[0] == 3.0f);
    CHECK(d.Attr(1)[0] == 4.0f);
    CHECK(d.Attr(2)[0] == 5.0f);
    CHECK(d.out.indices == Indices{0, 2, 1});
}

TEST_CASE("a stream with no buffer, or a draw with none, assembles nothing") {
    Draw d;
    d.s->vs_handle = 0x2;
    CHECK_FALSE(Assemble(*d.s, 5, 0, 3, nullptr, d.out));
}

TEST_CASE("pretransformed FVF positions are flagged") {
    Draw d;
    std::vector<float> positions(4 * 3, 0.0f);
    d.Buffer(positions, 16);
    d.s->vs_handle = 0x4;  // XYZRHW
    REQUIRE(Assemble(*d.s, 5, 0, 3, nullptr, d.out));
    CHECK(d.out.pretransformed);
    CHECK(d.out.elements[0].format == DXGI_FORMAT_R32G32B32A32_FLOAT);
}
