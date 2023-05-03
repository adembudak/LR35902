#include <LR35902/cartridge/header/header.h>
#include <LR35902/memory_map.h>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <range/v3/to_container.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/map.hpp>

#include <array>
#include <map>
#include <string>

namespace {
const std::map<std::string, LR35902::byte> kind{
    {"rom_only",                       0x00},
    {"mbc1",                           0x01},
    {"mbc1_ram",                       0x02},
    {"mbc1_ram_battery",               0x03},
    {"mbc2",                           0x05},
    {"mbc2_battery",                   0x06},
    {"rom_ram",                        0x08},
    {"rom_ram_battery",                0x09},
    {"mmm01",                          0x0b},
    {"mmm01_ram",                      0x0c},
    {"mmm01_ram_battery",              0x0d},
    {"mbc3_timer_battery",             0x0f},
    {"mbc3_timer_ram_battery",         0x10},
    {"mbc3",                           0x11},
    {"mbc3_ram",                       0x12},
    {"mbc3_ram_battery",               0x13},
    {"mbc5",                           0x19},
    {"mbc5_ram",                       0x1a},
    {"mbc5_ram_battery",               0x1b},
    {"mbc5_rumble",                    0x1c},
    {"mbc5_rumble_ram",                0x1d},
    {"mbc5_rumble_ram_battery",        0x1e},
    {"mbc6",                           0x20},
    {"mbc7_sensor_rumble_ram_battery", 0x22},
    {"pocketCamera",                   0xfc},
    {"bandaiTama5",                    0xfd},
    {"HuC3",                           0xfe},
    {"HuC1_ram_battery",               0xff}
};

struct MBCKindValidator : public CLI::Validator {
  MBCKindValidator() {
    name_ = "mbc kind validator";

    func_ = [&](const std::string &str) -> std::string {
      if(!kind.contains(str)) return "Not a valid MBC type";
      return std::string{};
    };
  }
};

} // end namespace

int main(int argc, const char *const argv[]) {
  const static MBCKindValidator mbc_kind_validator;

  using namespace LR35902;
  CLI::App app;

  std::fstream stream;

  try {
    std::string rom_file;
    std::string title;
    std::string mbc_type;
    byte version;
    byte rom_size;

    const auto rom = app.add_option("rom.gb", rom_file)->required(true)->check(CLI::ExistingFile);
    const auto fix_logo = app.add_flag("--fix-logo", "Fix logo");
    const auto set_title = app.add_option("--set-title", title, "Set rom title");
    const auto fix_csum = app.add_flag("--fix-csum", "Fix checksum");
    const auto set_version = app.add_option("--set-ver", version, "Set version")->check(CLI::Range(0x00, 0xff));
    const auto set_rom =
        app.add_option("--set-rom", rom_size, "Set cartridge ROM size")->check(CLI::Range(0x00, 0x08));
    const auto set_mbc = app.add_option("--set-mbc", mbc_type)->check(mbc_kind_validator)->description([&]() {
      std::ostringstream sout;
      sout << "Set memory bank controller type. ";
      sout << "Valid values: " << '\n';
      sout << (kind | ranges::views::keys | ranges::views::intersperse("\n") | ranges::views::join |
               ranges::to<std::string>);

      return sout.str();
    }());

    app.parse(argc, argv);

    if(*rom) {
      const auto stream_flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
      stream.open(rom_file, stream_flags);
    }

    if(*fix_logo) {
      stream.seekg(mmap::logo_begin);
      stream.write(reinterpret_cast<const char *>(data(nintendo_logo)), size(nintendo_logo));
    }

    if(*set_title) {
      title.resize(mmap::title_end - mmap::title_begin);
      stream.seekg(mmap::title_begin);
      stream.write(reinterpret_cast<const char *>(data(title)), size(title));
    }

    if(*fix_csum) {
      std::array<byte, mmap::csum_end - mmap::csum_begin> buf;

      stream.seekg(mmap::csum_begin);
      stream.read(reinterpret_cast<char *>(data(buf)), size(buf));

      const byte csum_result =
          std::accumulate(cbegin(buf), cend(buf), byte{0}, [](byte a, byte b) { return a - b - 1; });

      stream.seekg(mmap::csum_result);
      stream.write(reinterpret_cast<const char *>(&csum_result), sizeof(decltype(csum_result)));
    }

    if(*set_version) {
      stream.seekg(mmap::game_version);
      stream.write(reinterpret_cast<const char *>(&version), sizeof(decltype(version)));
    }

    if(*set_mbc) {
      stream.seekg(mmap::mbc_code);
      const byte code = kind.find(mbc_type)->second;
      stream.write(reinterpret_cast<const char *>(&code), sizeof(decltype(code)));
    }

    if(*set_rom) {
      stream.seekg(mmap::rom_code);
      stream.write(reinterpret_cast<const char *>(&rom_size), sizeof(decltype(rom_size)));
    }
  }
  catch(const CLI::ParseError &e) {
    return app.exit(e);
  }
}
