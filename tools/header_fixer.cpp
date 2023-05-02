#include <LR35902/cartridge/header/header.h>
#include <LR35902/memory_map.h>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <array>
#include <string>

int main(int argc, const char *const argv[]) {
  using namespace LR35902;
  CLI::App app;

  std::fstream stream;

  // manufacturarer
  // cgb-support
  // new licensee code
  // sgb support
  // mbc code
  // rom code
  // ram code
  // destination
  // old licensee

  try {
    std::string rom_file;
    std::string title;
    byte version;

    const auto rom = app.add_option("rom.gb", rom_file)->required(true)->check(CLI::ExistingFile);
    const auto fix_logo = app.add_flag("--fix-logo", "Fix logo");
    const auto set_title = app.add_option("--set-title", title, "Set rom title");
    const auto fix_csum = app.add_flag("--fix-csum", "Fix checksum");
    const auto set_version = app.add_option("--set-ver", version, "Set version")->check(CLI::Range(0x00, 0xff));

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

      const auto csum_result =
          std::accumulate(cbegin(buf), cend(buf), byte{0}, [](byte a, byte b) { return a - b - 1; });

      stream.seekg(mmap::csum_result);
      stream.write(reinterpret_cast<const char *>(&csum_result), sizeof(csum_result));
    }

    if(*set_version) {
      stream.seekg(mmap::game_version);
      stream.write(reinterpret_cast<const char *>(&version), sizeof(version));
    }

    //
  }
  catch(const CLI::ParseError &e) {
    return app.exit(e);
  }
}
