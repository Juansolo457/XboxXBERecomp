// DirectSound's decoder and cursors are private to its source, so this file is its translation unit.
#include "xdk/dsound.cpp"

#include <cmath>
#include <vector>

#include "support.h"

namespace {

constexpr uint32_t kBlockBytes = 36;

// one channel's block: predictor, step index, then 8 dwords of nibbles
std::vector<uint8_t> Block(int16_t pred, uint8_t idx, uint8_t fill) {
    std::vector<uint8_t> b(kBlockBytes, fill);
    b[0] = uint8_t(pred & 0xFF);
    b[1] = uint8_t((pred >> 8) & 0xFF);
    b[2] = idx;
    b[3] = 0;
    return b;
}

// the IMA ADPCM encoder of the format's definition: a nibble from the difference, then the state the decoder will have
struct Encoder {
    int pred = 0, idx = 0;
    uint8_t Encode(int sample) {
        int step = kStep[idx], delta = sample - pred;
        uint8_t n = delta < 0 ? 8 : 0;
        if (delta < 0) delta = -delta;
        int diff = step >> 3;
        if (delta >= step) {
            n |= 4;
            delta -= step;
            diff += step;
        }
        if (delta >= step >> 1) {
            n |= 2;
            delta -= step >> 1;
            diff += step >> 1;
        }
        if (delta >= step >> 2) {
            n |= 1;
            diff += step >> 2;
        }
        pred = std::clamp(n & 8 ? pred - diff : pred + diff, -32768, 32767);
        idx = std::clamp(idx + kIndex[n], 0, 88);
        return n;
    }
};

}  // namespace

TEST_CASE("a block's first frame is its predictor and its nibbles step from there, low nibble first") {
    auto b = Block(0, 0, 0x77);
    int16_t out[65];
    DecodeBlock(b.data(), 1, out);
    CHECK(out[0] == 0);
    CHECK(out[1] == 11);   // step 7: 0 + 1 + 3 + 7
    CHECK(out[2] == 41);   // index 8, step 16: 2 + 4 + 8 + 16
    CHECK(out[3] == 104);  // index 16, step 34: 4 + 8 + 17 + 34

    b = Block(100, 0, 0);
    b[4] = 0x07;  // only the first nibble moves it: the zero nibbles after it add step >> 3
    DecodeBlock(b.data(), 1, out);
    CHECK(out[1] == 111);
    CHECK(out[2] == 111 + (16 >> 3));
}

TEST_CASE("the sign nibble subtracts") {
    auto b = Block(1000, 0, 0xFF);
    int16_t out[65];
    DecodeBlock(b.data(), 1, out);
    CHECK(out[1] == 1000 - 11);
    CHECK(out[2] == 1000 - 11 - 30);
}

TEST_CASE("the predictor clamps to 16 bits and a bad step index to the table") {
    int16_t out[65];
    auto high = Block(32767, 88, 0x77);
    DecodeBlock(high.data(), 1, out);
    for (int16_t s : out) CHECK(s == 32767);
    auto low = Block(-32768, 200, 0xFF);
    DecodeBlock(low.data(), 1, out);
    for (int16_t s : out) CHECK(s == -32768);
}

TEST_CASE("stereo blocks hold a header per channel and interleave dwords of nibbles") {
    std::vector<uint8_t> b(kBlockBytes * 2, 0);
    b[0] = 100;  // left predictor
    int16_t right = -100;
    std::memcpy(&b[4], &right, 2);
    b[8] = 0x07;  // the left channel's first nibble
    int16_t out[130];
    DecodeBlock(b.data(), 2, out);
    CHECK(out[0] == 100);
    CHECK(out[1] == -100);
    CHECK(out[2] == 111);
    CHECK(out[3] == -100);
}

TEST_CASE("the decoder follows the format's encoder") {
    // a 1 kHz tone at 48 kHz through 20 blocks
    std::vector<int> pcm;
    double noise = 0, signal = 0;
    Encoder enc;
    for (int block = 0; block < 20; block++) {
        std::vector<uint8_t> b(kBlockBytes, 0);
        int start = int(16000 * std::sin(2 * 3.14159265358979 * 1000.0 * (block * 64) / 48000.0));
        enc.pred = start;
        int16_t pred = int16_t(start);
        std::memcpy(&b[0], &pred, 2);
        b[2] = uint8_t(enc.idx);
        std::vector<int> expected = {start}, input = {start};
        for (int k = 0; k < 64; k++) {
            int s = int(16000 * std::sin(2 * 3.14159265358979 * 1000.0 * (block * 64 + k + 1) / 48000.0));
            uint8_t n = enc.Encode(s);
            b[4 + k / 2] |= uint8_t(n << ((k & 1) * 4));
            expected.push_back(enc.pred);
            input.push_back(s);
        }
        int16_t out[65];
        DecodeBlock(b.data(), 1, out);
        for (int i = 0; i < 65; i++) {
            REQUIRE(out[i] == expected[size_t(i)]);
            if (block) {  // the first block settles the step size
                signal += double(input[size_t(i)]) * input[size_t(i)];
                noise += double(input[size_t(i)] - out[i]) * (input[size_t(i)] - out[i]);
            }
        }
    }
    CHECK(10 * std::log10(signal / noise) > 20.0);  // decibels of the tone over the error
}

TEST_CASE("sizes in frames and bytes") {
    Voice v;
    v.tag = 0x69;
    v.channels = 2;
    CHECK(FramesOf(v, 72 * 3) == 64 * 3);
    CHECK(BytesOf(v, 64 * 3) == 72 * 3);
    Voice pcm;
    pcm.align = 4;
    CHECK(FramesOf(pcm, 400) == 100);
    CHECK(BytesOf(pcm, 100) == 400);
}

TEST_CASE("samples are read through the block cache") {
    uint32_t data = mem::AllocSystem(kBlockBytes * 3);
    for (uint32_t i = 0; i < 3; i++) {
        auto b = Block(int16_t(1000 * (i + 1)), 0, 0);
        std::memcpy(kernel::HostPtr(data + i * kBlockBytes), b.data(), kBlockBytes);
    }
    Voice v;
    v.tag = 0x69;
    v.channels = 1;
    CHECK(Sample(v, data, 0, 0) == 1000 / 32768.0f);
    CHECK(Sample(v, data, 64, 0) == 2000 / 32768.0f);
    CHECK(Sample(v, data, 0, 0) == 1000 / 32768.0f);  // both still cached
    CHECK(Sample(v, data, 128, 0) == 3000 / 32768.0f);
    CHECK(v.block[v.last] == int64_t(data) + 2 * kBlockBytes);
    mem::FreeSystem(data);
}

TEST_CASE("PCM samples scale to one, and a missing channel reads the last") {
    uint32_t data = mem::AllocSystem(16);
    int16_t frame[2] = {16384, -32768};
    std::memcpy(kernel::HostPtr(data), frame, 4);
    Voice v;
    v.channels = 2;
    v.align = 4;
    CHECK(Sample(v, data, 0, 0) == 0.5f);
    CHECK(Sample(v, data, 0, 1) == -1.0f);
    CHECK(Sample(v, data, 0, 5) == -1.0f);
    Voice byte;
    byte.bits = 8;
    byte.align = 1;
    kernel::G<uint8_t>(data + 8) = 192;
    CHECK(Sample(byte, data + 8, 0, 0) == 0.5f);
    mem::FreeSystem(data);
}

// The play cursor through a looping buffer, frame by frame, against the position worked out without the wrap the source
// does. At f7b13fd the SDK has no SetNotificationPositions, so the cursor is what can be checked here.
TEST_CASE("a looping cursor stays in its region at any rate") {
    const double rates[] = {0.25, 0.5, 1.0, 1.5, 2.75, 7.0, 40.0};
    const uint32_t lengths[] = {1, 2, 3, 7, 16, 64, 100};
    const uint32_t starts[] = {0, 1, 5, 64};
    uint64_t checks = 0;
    uint32_t wrong = 0;
    for (double rate : rates)
        for (uint32_t len : lengths)
            for (uint32_t start : starts) {
                Voice v;
                v.align = 2;
                v.data = 0x1000;
                v.play_start = 0x40;
                v.looping = true;
                v.loop_start = start * 2;
                v.loop_len = len * 2;
                v.play_len = (start + len + 3) * 2;
                v.playing = true;
                const uint32_t end = start + len;
                double unwrapped = 0;
                for (int step = 0; step < 3000; step++) {
                    double expected = unwrapped < end ? unwrapped : start + std::fmod(unwrapped - start, double(len));
                    auto at = BufferFrame(v);
                    checks++;
                    uint32_t f = uint32_t(expected);
                    wrong += !at || at->f != f || at->data != v.data + v.play_start ||
                             at->next_f != (f + 1 < end ? f + 1 : start) || !v.playing || v.pos != expected;
                    unwrapped += rate;
                    v.pos += rate;
                }
            }
    CHECK(checks == 588000);
    CHECK(wrong == 0);
}

TEST_CASE("a buffer that does not loop stops at the end of its play region") {
    Voice v;
    v.align = 2;
    v.data = 0x1000;
    v.play_len = 20;
    v.playing = true;
    v.pos = 9.5;
    auto at = BufferFrame(v);
    REQUIRE(at);
    CHECK(at->f == 9);
    CHECK(at->next_f == 9);  // nothing after the last frame
    v.pos = 10;
    CHECK_FALSE(BufferFrame(v));
    CHECK_FALSE(v.playing);
    CHECK(v.pos == 0);
}

TEST_CASE("a loop with nothing in it stops") {
    Voice v;
    v.align = 2;
    v.data = 0x1000;
    v.looping = true;
    v.loop_start = 8;
    v.loop_len = 0;
    v.playing = true;
    v.pos = 4;
    CHECK_FALSE(BufferFrame(v));
    CHECK_FALSE(v.playing);
}

TEST_CASE("an ADPCM loop counts in blocks") {
    Voice v;
    v.tag = 0x69;
    v.channels = 1;
    v.data = 0x1000;
    v.looping = true;
    v.loop_start = kBlockBytes;
    v.loop_len = kBlockBytes * 2;
    v.playing = true;
    v.pos = 64 * 3;  // the loop's end
    auto at = BufferFrame(v);
    REQUIRE(at);
    CHECK(at->f == 64);
}

TEST_CASE("a stream walks its packets and completes each") {
    uint32_t status = mem::AllocSystem(16), completed = status + 4;
    kernel::W32(status, 0xFFFFFFFF);
    auto event = std::make_shared<kernel::Object>(kernel::ObjType::Event);
    uint32_t handle = kernel::NewHandle(event);

    Voice v;
    v.stream = true;
    v.align = 2;
    Packet first{}, second{};
    first.data = 0x1000;
    first.size = 8;
    first.status_ptr = status;
    first.completed_ptr = completed;
    first.context = handle;
    second.data = 0x2000;
    second.size = 8;
    v.packets = {first, second};

    v.pos = 2;
    auto at = StreamFrame(v);
    REQUIRE(at);
    CHECK(at->data == 0x1000);
    CHECK(at->f == 2);
    CHECK(at->next_f == 3);

    v.pos = 3;
    at = StreamFrame(v);
    REQUIRE(at);
    CHECK(at->next_data == 0x2000);  // the last frame reads on into the next packet
    CHECK(at->next_f == 0);
    CHECK(event->signal == 0);

    v.pos = 5;
    at = StreamFrame(v);
    REQUIRE(at);
    CHECK(at->data == 0x2000);
    CHECK(at->f == 1);
    CHECK(v.packets.size() == 1);
    CHECK(kernel::R32(status) == 0);
    CHECK(kernel::R32(completed) == 8);
    CHECK(event->signal == 1);

    v.active = v.ending = true;
    v.pos = 4;
    CHECK_FALSE(StreamFrame(v));
    CHECK(v.packets.empty());
    CHECK(v.pos == 0);
    CHECK_FALSE(v.active);  // it went quiet after its last packet
    kernel::CloseHandle(handle);
    mem::FreeSystem(status);
}
