// Cartridge header ([0x100, 0x150)) decoder, DMG only
// Based on: https://gbdev.io/pandocs/The_Cartridge_Header.html#the-cartridge-header

#include <algorithm> // for std::equal
#include <array>
#include <cstdint> // for std::size_t
#include <filesystem>
#include <fstream>
#include <numeric> // for std::accumulate
#include <string>
#include <unordered_map>
#include <vector>

#include <CLI/CLI.hpp>
#include <fmt/core.h>

using namespace std::string_literals;
namespace fs = std::filesystem;

using byte = std::uint8_t;

namespace {

constexpr std::size_t cartridge_header_begin = 0x100;
constexpr std::size_t cartridge_header_end = 0x14f + 1;

constexpr std::size_t logo_begin = 0x104;
constexpr std::size_t logo_end = 0x133 + 1;
constexpr std::array<byte, logo_end - logo_begin> nintendo_logo{
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
    0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
    0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

constexpr std::size_t title_begin = 0x134;
constexpr std::size_t title_end = 0x143 + 1;
constexpr std::size_t manufacturer_code_begin = 0x13f;
constexpr std::size_t manufacturer_code_end = 0x142 + 1;
constexpr std::size_t color_gameboy_support = 0x143;
constexpr std::size_t new_licensee_code0 = 0x144;
constexpr std::size_t new_licensee_code1 = 0x145;
constexpr std::size_t super_gameboy_support = 0x146;
constexpr std::size_t mbc_code = 0x147;
constexpr std::size_t rom_size = 0x148;
constexpr std::size_t ram_size = 0x149;
constexpr std::size_t destination_code = 0x14a;
constexpr std::size_t old_licensee_code = 0x14b;
constexpr std::size_t game_version = 0x14c;

constexpr std::size_t checksum_begin = 0x134;
constexpr std::size_t checksum_end = 0x14c + 1;
constexpr std::size_t checksum_result = 0x14d;

std::string destination_name(const byte b) {
  switch(b) {
  case 0x00: return "jp";
  case 0x01: return "other";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string sgb_support(const byte b) {
  switch(b) {
  case 0x00: return "compatible";
  case 0x03: return "need sgb functionalities";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string cgb_support(const byte b) {
  switch(b) {
  case 0x00: return "dmg-only";
  case 0x80: return "dmg-compatible";
  case 0xc0: return "cgb-only";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string mbc_type(const byte b) {
  switch(b) {
  case 0x00: return "rom_only";
  case 0x01: return "mbc1";
  case 0x02: return "mbc1+ram";
  case 0x03: return "mbc1+ram+battery";
  case 0x05: return "mbc2";
  case 0x06: return "mbc2+battery";
  case 0x08: return "rom+ram";
  case 0x09: return "rom+ram+battery";
  case 0x0b: return "mmm01";
  case 0x0c: return "mmm01+ram";
  case 0x0d: return "mmm01+ram+battery";
  case 0x0f: return "mbc3+timer+battery";
  case 0x10: return "mbc3+timer+ram+battery";
  case 0x11: return "mbc3";
  case 0x12: return "mbc3+ram";
  case 0x13: return "mbc3+ram+battery";
  case 0x19: return "mbc5";
  case 0x1a: return "mbc5+ram";
  case 0x1b: return "mbc5+ram+battery";
  case 0x1c: return "mbc5+rumble";
  case 0x1d: return "mbc5+rumble+ram";
  case 0x1e: return "mbc5+rumble+ram+battery";
  case 0x20: return "mbc6";
  case 0x22: return "mbc7+sensor+rumble+ram+battery";
  case 0xfc: return "pocket camera";
  case 0xfd: return "bandai tama5";
  case 0xfe: return "HuC3";
  case 0xff: return "HuC1+ram+battery";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string rom_type(const byte b) {
  switch(b) {
  case 0x00: return "32_kb";
  case 0x01: return "64_kb";
  case 0x02: return "128_kb";
  case 0x03: return "256_kb";
  case 0x04: return "512_kb";
  case 0x05: return "1_mb";
  case 0x06: return "2_mb";
  case 0x07: return "4_mb";
  case 0x08: return "8_mb";
  case 0x52: return "1.1_mb";
  case 0x53: return "1.2_mb";
  case 0x54: return "1.5_mb";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string ram_type(const byte b) {
  switch(b) {
  case 0x00: return "No RAM";
  case 0x01: return "-";
  case 0x02: return "8_kb";
  case 0x03: return "32_kb";
  case 0x04: return "128_kb";
  case 0x05: return "64_kb";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

std::string new_licensee_name(const byte a, const byte c) {
  const std::string s{static_cast<char>(a), static_cast<char>(c)};

  if(s == "00") return "None";
  else if(s == "01") return "Nintendo";
  else if(s == "08") return "Capcom";
  else if(s == "13") return "Electronic arts";
  else if(s == "18") return "Hudsonsoft";
  else if(s == "19") return "B-ai";
  else if(s == "20") return "Kss";
  else if(s == "22") return "Pow";
  else if(s == "24") return "Pcm complete";
  else if(s == "25") return "San-x";
  else if(s == "28") return "Kemco japan";
  else if(s == "29") return "Seta";
  else if(s == "30") return "Viacom";
  else if(s == "31") return "Nintendo";
  else if(s == "32") return "Bandia";
  else if(s == "33") return "Ocean/acclaim";
  else if(s == "34") return "Konami";
  else if(s == "35") return "Hector";
  else if(s == "37") return "Taito";
  else if(s == "38") return "Hudson";
  else if(s == "39") return "Banpresto";
  else if(s == "41") return "Ubi soft";
  else if(s == "42") return "Atlus";
  else if(s == "44") return "Malibu";
  else if(s == "46") return "Angel";
  else if(s == "47") return "Pullet-proof";
  else if(s == "49") return "Irem";
  else if(s == "50") return "Absolute";
  else if(s == "51") return "Acclaim";
  else if(s == "52") return "Activision";
  else if(s == "53") return "American sammy";
  else if(s == "54") return "Konami";
  else if(s == "55") return "Hi tech entertainment";
  else if(s == "56") return "Ljn";
  else if(s == "57") return "Matchbox";
  else if(s == "58") return "Mattel";
  else if(s == "59") return "Milton bradley";
  else if(s == "60") return "Titus";
  else if(s == "61") return "Virgin";
  else if(s == "64") return "Lucasarts";
  else if(s == "67") return "Ocean";
  else if(s == "69") return "Electronic arts";
  else if(s == "70") return "Infogrames";
  else if(s == "71") return "Interplay";
  else if(s == "72") return "Broderbund";
  else if(s == "73") return "Sculptured";
  else if(s == "75") return "Sci";
  else if(s == "78") return "T*hq";
  else if(s == "79") return "Accolade";
  else if(s == "80") return "Misawa";
  else if(s == "83") return "Lozc";
  else if(s == "86") return "Tokuma shoten i*";
  else if(s == "87") return "Tsukuda original";
  else if(s == "91") return "Chun soft";
  else if(s == "92") return "Video system";
  else if(s == "93") return "Ocean/acclaim";
  else if(s == "95") return "Varie";
  else if(s == "96") return "Yonezawa/s'pal";
  else if(s == "97") return "Kaneko";
  else if(s == "99") return "Pack in soft";
  else if(s == "A4") return "Konami (Yu-Gi-Oh!)";
  else return "Unknown";
}

std::string licensee_name(byte b, [[maybe_unused]] byte c, [[maybe_unused]] byte d) {
  if(b == 0x33) return new_licensee_name(c, d);

  switch(b) {
  case 0x00: return "None";
  case 0x01: return "Nintendo";
  case 0x08: return "Capcom";
  case 0x09: return "Hot-b";
  case 0x0a: return "Jaleco";
  case 0x0b: return "Coconuts";
  case 0x0c: return "Elite systems";
  case 0x13: return "Electronic arts";
  case 0x18: return "Hudsonsoft";
  case 0x19: return "Itc entertainment";
  case 0x1a: return "Yanoman";
  case 0x1d: return "Clary";
  case 0x1f: return "Virgin";
  case 0x24: return "Pcm complete";
  case 0x25: return "San-x";
  case 0x28: return "Kotobuki systems";
  case 0x29: return "Seta";
  case 0x30: return "Infogrames";
  case 0x31: return "Nintendo";
  case 0x32: return "Bandai";
  case 0x34: return "Konami";
  case 0x35: return "Hector";
  case 0x38: return "Capcom";
  case 0x39: return "Banpresto";
  case 0x3c: return "*entertainment i";
  case 0x3e: return "Gremlin";
  case 0x41: return "Ubi soft";
  case 0x42: return "Atlus";
  case 0x44: return "Malibu";
  case 0x46: return "Angel";
  case 0x47: return "Spectrum holoby";
  case 0x49: return "Irem";
  case 0x4a: return "Virgin";
  case 0x4d: return "Malibu";
  case 0x4f: return "U.s. gold";
  case 0x50: return "Absolute";
  case 0x51: return "Acclaim";
  case 0x52: return "Activision";
  case 0x53: return "American sammy";
  case 0x54: return "Gametek";
  case 0x55: return "Park place";
  case 0x56: return "Ljn";
  case 0x57: return "Matchbox";
  case 0x59: return "Milton bradley";
  case 0x5a: return "Mindscape";
  case 0x5b: return "Romstar";
  case 0x5c: return "Naxat soft";
  case 0x5d: return "Tradewest";
  case 0x60: return "Titus";
  case 0x61: return "Virgin";
  case 0x67: return "Ocean";
  case 0x69: return "Electronic arts";
  case 0x6e: return "Elite systems";
  case 0x6f: return "Electro brain";
  case 0x70: return "Infogrames";
  case 0x71: return "Interplay";
  case 0x72: return "Broderbund";
  case 0x73: return "Sculptered soft";
  case 0x75: return "The sales curve";
  case 0x78: return "T*hq";
  case 0x79: return "Accolade";
  case 0x7a: return "Triffix entertainment";
  case 0x7c: return "Microprose";
  case 0x7f: return "Kemco";
  case 0x80: return "Misawa entertainment";
  case 0x83: return "Lozc";
  case 0x86: return "*tokuma shoten i";
  case 0x8b: return "Bullet-proof software";
  case 0x8c: return "Vic tokai";
  case 0x8e: return "Ape";
  case 0x8f: return "I'max";
  case 0x91: return "Chun soft";
  case 0x92: return "Video system";
  case 0x93: return "Tsuburava";
  case 0x95: return "Varie";
  case 0x96: return "Yonezawa/s'pal";
  case 0x97: return "Kaneko";
  case 0x99: return "Arc";
  case 0x9a: return "Nihon bussan";
  case 0x9b: return "Tecmo";
  case 0x9c: return "Imagineer";
  case 0x9d: return "Banpresto";
  case 0x9f: return "Nova";
  case 0xa1: return "Hori electric";
  case 0xa2: return "Bandai";
  case 0xa4: return "Konami";
  case 0xa6: return "Kawada";
  case 0xa7: return "Takara";
  case 0xa9: return "Technos japan";
  case 0xaa: return "Broderbund";
  case 0xac: return "Toei animation";
  case 0xad: return "Toho";
  case 0xaf: return "Namco";
  case 0xb0: return "Acclaim";
  case 0xb1: return "Ascii or nexoft";
  case 0xb2: return "Bandai";
  case 0xb4: return "Enix";
  case 0xb6: return "Hal";
  case 0xb7: return "Snk";
  case 0xb9: return "Pony canyon";
  case 0xba: return "*culture brain o";
  case 0xbb: return "Sunsoft";
  case 0xbd: return "Sony imagesoft";
  case 0xbf: return "Sammy";
  case 0xc0: return "Taito";
  case 0xc2: return "Kemco";
  case 0xc3: return "Squaresoft";
  case 0xc4: return "*tokuma shoten i";
  case 0xc5: return "Data east";
  case 0xc6: return "Tonkin house";
  case 0xc8: return "Koei";
  case 0xc9: return "Ufl";
  case 0xca: return "Ultra";
  case 0xcb: return "Vap";
  case 0xcc: return "Use";
  case 0xcd: return "Meldac";
  case 0xce: return "*pony canyon or";
  case 0xcf: return "Angel";
  case 0xd0: return "Taito";
  case 0xd1: return "Sofel";
  case 0xd2: return "Quest";
  case 0xd3: return "Sigma enterprises";
  case 0xd4: return "Ask kodansha";
  case 0xd6: return "Naxat soft";
  case 0xd7: return "Copya systems";
  case 0xd9: return "Banpresto";
  case 0xda: return "Tomy";
  case 0xdb: return "Ljn";
  case 0xdd: return "Ncs";
  case 0xde: return "Human";
  case 0xdf: return "Altron";
  case 0xe0: return "Jaleco";
  case 0xe1: return "Towachiki";
  case 0xe2: return "Uutaka";
  case 0xe3: return "Varie";
  case 0xe5: return "Epoch";
  case 0xe7: return "Athena";
  case 0xe8: return "Asmik";
  case 0xe9: return "Natsume";
  case 0xea: return "King records";
  case 0xeb: return "Atlus";
  case 0xec: return "Epic/sony records";
  case 0xee: return "Igs";
  case 0xf0: return "A wave";
  case 0xf3: return "Extreme entertainment";
  case 0xff: return "Ljn";
  default: return "unknown value: "s.append(std::to_string(b));
  }
}

}

int main(int argc, const char *const argv[]) {
  CLI::App app;

  std::vector<fs::directory_entry> romEntries;
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

  for(const auto &e : romEntries) {
    if(!e.exists() || e.path().extension() != ".gb") continue;

    std::array<byte, cartridge_header_end> headerDump{};

    std::ifstream fin{e.path()};
    fin.read(std::bit_cast<char *>(headerDump.data()),
             cartridge_header_end); // read till cartridge header end
    fin.close();

    if(options[0]) {
      fmt::print("File name :       {}\n", e.path().filename().string());
      fmt::print("Title :           {}\n", std::string(&headerDump[title_begin], &headerDump[title_end]));
    }

    if(options[1]) {
      fmt::print("Physical size :   {}\n", e.file_size());
      fmt::print("ROM :             {}\n", rom_type(headerDump[rom_size]));
      fmt::print("RAM :             {}\n", ram_type(headerDump[ram_size]));
    }

    if(options[2]) {
      fmt::print("MBC :             {}\n", mbc_type(headerDump[mbc_code]));
    }

    if(options[3]) {
      fmt::print("Destination :     {}\n", destination_name(headerDump[destination_code]));
    }

    if(options[4]) {
      fmt::print("Color GB :        {}\n", cgb_support(headerDump[color_gameboy_support]));
    }

    if(options[5]) {
      fmt::print("Super GB :        {}\n", sgb_support(headerDump[super_gameboy_support]));
    }

    if(options[6]) {
      const bool logo_check =
          std::equal(&headerDump[logo_begin], &headerDump[logo_end], nintendo_logo.begin());

      fmt::print("Logo check :      {}\n", logo_check ? "Passed" : "Failed");

      const int checksum = std::accumulate(&headerDump[checksum_begin], &headerDump[checksum_end], 0,
                                           [](byte x, byte y) { return x - y - 1; });

      fmt::print("Header checksum : {}\n", checksum == headerDump[checksum_result] ? "Passed" : "Failed");
    }

    if(options[7]) {
      fmt::print("Publisher :       {}\n",
                 licensee_name(headerDump[old_licensee_code], headerDump[new_licensee_code0],
                               headerDump[new_licensee_code1]));
    }

    if(options[8]) {
      fmt::print("Version :         {}\n", headerDump[game_version]);
    }

    fmt::print("\n");
  }
}
