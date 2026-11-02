#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "support.h"

namespace fs = std::filesystem;

namespace {

constexpr uint32_t kNtClose = 187, kNtCreateFile = 190, kNtOpenFile = 202, kNtQueryDirectoryFile = 207,
                   kNtSetInformationFile = 226;
constexpr uint32_t kDirectoryOption = 1;
constexpr uint32_t kFileDirectoryInformation = 1;
constexpr uint32_t kFileRenameInformation = 10;

// game files under D:, a hard disk under the data folder
struct Mounts {
    fs::path root, game, data;
    Mounts() {
        std::random_device rd;
        root = fs::temp_directory_path() / ("xbe_files_test_" + std::to_string(rd()));
        game = root / "game";
        data = root / "data";
        fs::create_directories(game / "sub");
        for (const char* name : {"ABC.TXT", "abd.txt", "readme.md"}) std::ofstream(game / name) << "x";
        kernel::MountDefaults(game.string(), data.string());
    }
    ~Mounts() {
        std::error_code ec;
        fs::remove_all(root, ec);
    }
};

Mounts& GetMounts() {
    static Mounts m;
    return m;
}

// ANSI_STRING with its text, in guest memory
uint32_t AnsiString(const std::string& s) {
    uint32_t str = mem::AllocSystem(16 + uint32_t(s.size()) + 1);
    kernel::W16(str, uint16_t(s.size()));
    kernel::W16(str + 2, uint16_t(s.size() + 1));
    kernel::W32(str + 4, str + 16);
    std::memcpy(kernel::HostPtr(str + 16), s.data(), s.size());
    return str;
}

struct Call : test::GuestStack {
    uint32_t iosb = mem::AllocSystem(8);
    uint32_t out = mem::AllocSystem(0x200);

    uint32_t Open(const std::string& path, uint32_t options, uint32_t* status = nullptr) {
        uint32_t attrs = mem::AllocSystem(12), handle = mem::AllocSystem(4);
        kernel::W32(attrs, 0);
        kernel::W32(attrs + 4, AnsiString(path));
        uint32_t r = test::KernelCall(ctx, kNtOpenFile, {handle, 0x80000000u, attrs, iosb, 7, options});
        if (status) *status = r;
        else REQUIRE(r == kernel::kSuccess);
        return kernel::R32(handle);
    }

    // one entry into out
    uint32_t Next(uint32_t dir, uint32_t mask, uint32_t restart = 0, uint32_t len = 0x200) {
        return test::KernelCall(ctx, kNtQueryDirectoryFile, {dir, 0, 0, 0, iosb, out, len, kFileDirectoryInformation, mask, restart});
    }

    std::string Name() const {
        return std::string(static_cast<const char*>(kernel::HostPtr(out + 0x40)), kernel::R32(out + 0x3C));
    }

    // every name a fresh handle lists for the mask, in lower case and sorted
    std::vector<std::string> List(uint32_t mask) {
        uint32_t dir = Open("\\??\\D:\\", kDirectoryOption);
        std::vector<std::string> names;
        while (Next(dir, mask) == kernel::kSuccess) {
            std::string n = Name();
            for (char& c : n) c = char(std::tolower(uint8_t(c)));
            names.push_back(n);
        }
        test::KernelCall(ctx, kNtClose, {dir});
        std::sort(names.begin(), names.end());
        return names;
    }
};

using Names = std::vector<std::string>;
const Names kAll = {"abc.txt", "abd.txt", "readme.md", "sub"};

}  // namespace

TEST_CASE("NtQueryDirectoryFile masks") {
    GetMounts();
    Call c;
    CHECK(c.List(0) == kAll);
    CHECK(c.List(AnsiString("*")) == kAll);
    CHECK(c.List(AnsiString("*.TXT")) == Names{"abc.txt", "abd.txt"});
    CHECK(c.List(AnsiString("*.txt")) == Names{"abc.txt", "abd.txt"});
    CHECK(c.List(AnsiString("a?c.txt")) == Names{"abc.txt"});
    CHECK(c.List(AnsiString("re*me.m?")) == Names{"readme.md"});
    CHECK(c.List(AnsiString("ABC.TXT")) == Names{"abc.txt"});
    CHECK(c.List(AnsiString("nothing*")).empty());
}

// SDK bug at f7b13fd: XAPI's FindFirstFile passes an empty mask for "*.*", which the host matches against nothing, so
// the title finds no files. should_fail keeps the suite green and fails it when the bug is fixed.
TEST_CASE("an empty mask lists everything, as no mask does" * doctest::should_fail()) {
    GetMounts();
    Call c;
    CHECK(c.List(AnsiString("")) == kAll);
}

TEST_CASE("the end of a listing and a restart") {
    GetMounts();
    Call c;
    uint32_t mask = AnsiString("abc.txt");
    uint32_t dir = c.Open("\\??\\D:\\", kDirectoryOption);
    CHECK(c.Next(dir, mask) == kernel::kSuccess);
    CHECK(c.Name() == "ABC.TXT");
    CHECK(c.Next(dir, mask) == kernel::kNoMoreFiles);
    CHECK(c.Next(dir, mask, 1) == kernel::kSuccess);  // RestartScan
    CHECK(c.Name() == "ABC.TXT");
    test::KernelCall(c.ctx, kNtClose, {dir});

    dir = c.Open("\\??\\D:\\", kDirectoryOption);
    CHECK(c.Next(dir, AnsiString("nothing*")) == kernel::kNoSuchFile);
    test::KernelCall(c.ctx, kNtClose, {dir});
}

TEST_CASE("an entry carries its name length and directory flag") {
    GetMounts();
    Call c;
    uint32_t dir = c.Open("\\??\\D:\\", kDirectoryOption);
    REQUIRE(c.Next(dir, AnsiString("sub")) == kernel::kSuccess);
    CHECK(kernel::R32(c.out + 0x3C) == 3);
    CHECK((kernel::R32(c.out + 0x38) & 0x10) != 0);  // FILE_ATTRIBUTE_DIRECTORY
    CHECK(kernel::R32(c.iosb + 4) == 0x43);          // bytes written, in the IO status block
    test::KernelCall(c.ctx, kNtClose, {dir});
}

TEST_CASE("a bad handle is refused") {
    Call c;
    CHECK(c.Next(0xDEAD0, 0) == kernel::kInvalidHandle);
}

namespace {

// a file on the first hard disk partition, renamed through its open handle
uint32_t Rename(Call& c, const std::string& from, const std::string& to, uint8_t replace) {
    uint32_t file = c.Open("\\Device\\Harddisk0\\Partition1\\" + from, 0);
    uint32_t in = mem::AllocSystem(16 + uint32_t(to.size()) + 40);
    std::string target = "\\Device\\Harddisk0\\Partition1\\" + to;
    kernel::G<uint8_t>(in) = replace;
    kernel::W32(in + 4, 0);
    kernel::W16(in + 8, uint16_t(target.size()));
    kernel::W16(in + 10, uint16_t(target.size() + 1));
    uint32_t text = mem::AllocSystem(uint32_t(target.size()) + 1);
    kernel::W32(in + 12, text);
    std::memcpy(kernel::HostPtr(text), target.data(), target.size());
    uint32_t r = test::KernelCall(c.ctx, kNtSetInformationFile, {file, c.iosb, in, 16, kFileRenameInformation});
    test::KernelCall(c.ctx, kNtClose, {file});
    return r;
}

}  // namespace

// SDK bug at f7b13fd, on Windows only: the handle's stream stays open across the rename and fopen's streams do not share
// delete, so a title's rename fails with access denied.
#ifdef _WIN32
#define XBE_WINDOWS_RENAME_BUG * doctest::should_fail()
#else
#define XBE_WINDOWS_RENAME_BUG
#endif
TEST_CASE("a file renames through its open handle" XBE_WINDOWS_RENAME_BUG) {
    Mounts& m = GetMounts();
    Call c;
    fs::path disk = m.data / "E";
    std::ofstream(disk / "old.dat") << "hello";
    CHECK(Rename(c, "old.dat", "new.dat", 1) == kernel::kSuccess);
    CHECK(fs::exists(disk / "new.dat"));
    CHECK_FALSE(fs::exists(disk / "old.dat"));
}

// SDK bug at f7b13fd: ReplaceIfExists is ignored, a rename onto an existing file replaces it.
TEST_CASE("a rename onto an existing file without ReplaceIfExists is a collision" * doctest::should_fail()) {
    Mounts& m = GetMounts();
    Call c;
    fs::path disk = m.data / "E";
    std::ofstream(disk / "first.dat") << "first";
    std::ofstream(disk / "second.dat") << "second";
    CHECK(Rename(c, "first.dat", "second.dat", 0) == kernel::kObjectNameCollision);
    CHECK(fs::exists(disk / "first.dat"));
}

TEST_CASE("opening what does not exist") {
    GetMounts();
    Call c;
    uint32_t status = 0;
    c.Open("\\??\\D:\\no such file", 0, &status);
    CHECK(status != kernel::kSuccess);
}
