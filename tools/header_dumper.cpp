
#include <LR35902/cartridge/header/header.h>

#include <CLI/CLI.hpp>
#include <fmt/core.h>
#include <range/v3/view/filter.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

int main(int argc, const char *const argv[]) {
  using namespace LR35902;

  CLI::App app;

  std::vector<std::filesystem::directory_entry> romEntries;
  app.add_option("rom.gb", romEntries, "rom file or directory");

  std::array<bool, 9> options{};
  app.add_flag("-t,--title", options[0], "Print cart title");
  app.add_flag("-m,--memory", options[1], "Print rom/ram size");
  app.add_flag("-c,--mbc", options[2], "Print memory bank controller type");
  app.add_flag("--destination", options[3], "Print cart destination");
  app.add_flag("--cgb", options[4], "Print color gameboy support");
  app.add_flag("--sgb", options[5], "Print super gameboy support");
  app.add_flag("--checksums", options[6], "Print cart version");
  app.add_flag("--publisher", options[7], "Print cart publisher");
  app.add_flag("--version", options[8], "Print cart version");

  CLI11_PARSE(app, argc, argv);

  if(std::ranges::none_of(options, [](bool b) { return b; })) options.fill(true);

  auto validEntry = [](const std::filesystem::directory_entry &e) noexcept -> bool {
    return exists(e.path()) && e.path().extension() == ".gb" && !is_empty(e.path());
  };

  for(const auto &e : romEntries | ranges::views::filter(validEntry)) {
    std::array<byte, cartridge_header_end> headerDump{};
    std::ifstream fin;

    fin.open(e.path());
    fin.read(std::bit_cast<char *>(headerDump.data()), cartridge_header_end);
    fin.close();

    LR35902::header_t header{headerDump};

    if(options[0]) {
      fmt::print("File name :       {}\n", e.path().filename().string());
      fmt::print("Title :           {}\n", header.decode_title());
    }

    if(options[1]) {
      fmt::print("Physical size :   {}\n", e.file_size());
      fmt::print("ROM :             {}\n", header.decode_rom_size().first);
      fmt::print("RAM :             {}\n", header.decode_ram_size().first);
    }

    if(options[2]) {
      fmt::print("MBC :             {}\n", header.decode_mbc_type().first);
    }

    if(options[3]) {
      fmt::print("Destination :     {}\n", header.decode_destination_name());
    }

    if(options[4]) {
      fmt::print("Color GB :        {}\n", header.decode_cgb_support());
    }

    if(options[5]) {
      fmt::print("Super GB :        {}\n", header.decode_sgb_support());
    }

    if(options[6]) {
      fmt::print("Logo check :      {}\n", header.is_logocheck_ok() ? "Passed" : "Failed");
      fmt::print("Header checksum : {}\n", header.is_checksum_ok() ? "Passed" : "Failed");
    }

    if(options[7]) {
      fmt::print("Publisher :       {}\n", header.decode_licensee_name());
    }

    if(options[8]) {
      fmt::print("Version :         {}\n", header.decode_version());
    }

    fmt::print("\n");
  }
}
