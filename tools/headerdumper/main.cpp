#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <CLI/CLI.hpp>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <ranges>

using namespace LR35902;

int main(int argc, const char *const argv[]) {
  CLI::App app;
  app.get_formatter()->column_width(20);

  std::vector<std::filesystem::directory_entry> romEntries;
  app.add_option("rom.gb", romEntries, "rom files");

  std::array<bool, 9> options{};
  app.add_flag("-t,--title", options[0], "Print cart title");
  app.add_flag("-m,--memory", options[1], "Print rom/ram size");
  app.add_flag("-c,--mbc", options[2], "Print memory bank controller type");
  app.add_flag("--destination", options[3], "Print cart destination");
  app.add_flag("--cgb", options[4], "Print color gameboy support");
  app.add_flag("--sgb", options[5], "Print super gameboy support");
  app.add_flag("--checksums", options[6], "Print logo check and header checksum");
  app.add_flag("--publisher", options[7], "Print cart publisher");
  app.add_flag("--version", options[8], "Print cart version");

  try {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError &pe) {
    return app.exit(pe);
  }

  if(std::ranges::none_of(options, [](bool b) { return b; })) options.fill(true);

  const auto validEntry = [](const std::filesystem::directory_entry &e) noexcept -> bool {
    return exists(e.path()) && (e.path().extension() == ".gb" || e.path().extension() == ".gbc") &&
           !is_empty(e.path());
  };

  for(const auto &e : romEntries | std::views::filter(validEntry)) {
    std::array<byte, mmap::header_end> headerDump;

    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit);

    try {
      fin.open(e.path());
      fin.read(reinterpret_cast<char *>(headerDump.data()), mmap::header_end);
      fin.close();

      fin.open("metadata.json");
      const auto metadata = nlohmann::json::parse(fin);
      fin.close();

      fmt::print("\n");

      if(options[0]) {
        fmt::print("  File name :       {}\n", e.path().filename().string());

        auto title = std::string(&headerDump[mmap::title_begin], &headerDump[mmap::title_end]);
        std::ranges::replace_if(title, [](char c) { return !std::isprint(c); }, ' ');
        fmt::print("  Title :           {}\n", title);
      }

      const auto hexToString = [](byte b) -> std::string { return fmt::format("{:#04x}", b); };

      if(options[1]) {
        fmt::print("  Physical size :   {}\n", e.file_size());

        const std::string rom_size = metadata["mbc"]["rom"]["size"][hexToString(headerDump[mmap::rom_code])];
        const std::string rom_banks = metadata["mbc"]["rom"]["bank"][hexToString(headerDump[mmap::rom_code])];
        fmt::print("  ROM :             {} ({})\n", rom_size, rom_banks);

        const std::string ram_size = metadata["mbc"]["ram"]["size"][hexToString(headerDump[mmap::ram_code])];
        const std::string ram_banks = metadata["mbc"]["ram"]["bank"][hexToString(headerDump[mmap::ram_code])];
        fmt::print("  RAM :             {} ({})\n", ram_size, ram_banks);
      }

      if(options[2]) {
        const std::string mbc_type = metadata["mbc"]["type"][hexToString(headerDump[mmap::mbc_code])];
        fmt::print("  MBC :             {}\n", mbc_type);
      }

      if(options[3]) {
        const std::string destination = metadata["destination"][hexToString(headerDump[mmap::destination])];
        fmt::print("  Destination :     {}\n", destination);
      }

      if(options[4]) {
        const std::string cgb_support = metadata["compat"]["cgb"][hexToString(headerDump[mmap::cgb_support])];
        fmt::print("  Color GB :        {}\n", cgb_support);
      }

      if(options[5]) {
        const std::string sgb_support = metadata["compat"]["sgb"][hexToString(headerDump[mmap::sgb_support])];
        fmt::print("  Super GB :        {}\n", sgb_support);
      }

      if(options[6]) {
        const bool is_logocheck_ok = std::equal(&headerDump[mmap::logo_begin], &headerDump[mmap::logo_end],
                                                metadata["nintendo_logo_blob"].begin());
        fmt::print("  Logo check :      {}\n", is_logocheck_ok ? "Passed" : "Failed");

        const int checksum = std::accumulate(&headerDump[mmap::csum_begin], &headerDump[mmap::csum_end], 0,
                                             [](byte x, byte y) { return x - y - 1; });
        const bool is_checksum_ok = checksum == headerDump[mmap::csum_result];
        fmt::print("  Header checksum : {}\n", is_checksum_ok ? "Passed" : "Failed");
      }

      if(options[7]) {
        const auto licensee = [&](const byte b) -> std::string {
          if(b == 0x33) {
            const std::string s{static_cast<char>(headerDump[mmap::new_licensee0]),
                                static_cast<char>(headerDump[mmap::new_licensee1])};
            return metadata["licensee"]["0x33"][s];
          }
          return metadata["licensee"][hexToString(b)];
        }(headerDump[mmap::old_licensee]);

        fmt::print("  Publisher :       {}\n", licensee);
      }

      if(options[8]) {
        fmt::print("  Version :         {}\n", headerDump[mmap::game_version]);
      }

      fmt::print("\n");
    }
    catch(const std::fstream::failure &f) {
      fmt::print("{}\n", f.what());
    }
    catch(const std::exception &e) {
      fmt::print("{}\n", e.what());
    }
  }
}
