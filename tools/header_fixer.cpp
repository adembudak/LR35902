#include <LR35902/cartridge/header/header.h>
#include <LR35902/memory_map.h>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <string>

int main(int argc, const char *const argv[]) {
  using namespace LR35902;
  CLI::App app;

  std::fstream stream;

  try {
    std::string rom_file;
    const auto rom = app.add_option("rom.gb", rom_file)->required(true)->check(CLI::ExistingFile);

    const auto fix_logo = app.add_flag("--fix-logo", "Fix logo");

    std::string title;
    const auto set_title = app.add_option("--set-title", title, "Set rom title");

    app.parse(argc, argv);

    if(*rom) {
      const auto stream_flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
      stream.open(rom_file, stream_flags);
    }

    if(*fix_logo) {
      stream.seekg(mmap::logo_begin);
      stream.write(reinterpret_cast<const char *>(nintendo_logo.data()), nintendo_logo.size());
    }

    if(*set_title) {
      title.resize(mmap::title_end - mmap::title_begin);
      stream.seekg(mmap::title_begin);
      stream.write(reinterpret_cast<const char *>(title.data()), title.size());
    }

    //
  }
  catch(const CLI::ParseError &e) {
    return app.exit(e);
  }
}
