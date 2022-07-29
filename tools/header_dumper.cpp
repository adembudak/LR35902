// Cartridge header ([0x100, 0x150)) decoder, DMG only
// Based on: https://gbdev.io/pandocs/The_Cartridge_Header.html#the-cartridge-header

#include <algorithm> // for std::replace, std::equal
#include <array>
#include <cstdint> // for std::size_t
#include <filesystem>
#include <fstream>
#include <numeric> // for std::accumulate
#include <string_view>
#include <unordered_map>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>

using byte = std::uint8_t;

constexpr std::size_t cartridge_header_begin = 0x100;
constexpr std::size_t cartridge_header_end = 0x14f + 1;

constexpr std::size_t nintendo_logo_begin = 0x104;
constexpr std::size_t nintendo_logo_end = 0x133 + 1;
static_assert((0x134 - 0x104) == 48);

constexpr std::array<byte, 48> nintendo_logo{
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
constexpr std::size_t mbc_type = 0x147;
constexpr std::size_t rom_size = 0x148;
constexpr std::size_t ram_size = 0x149;
constexpr std::size_t destination_code = 0x14a;
constexpr std::size_t old_licensee_code = 0x14b;
constexpr std::size_t game_version = 0x14c;

constexpr std::size_t header_checksum_begin = 0x134;
constexpr std::size_t header_checksum_end = 0x14c + 1;
constexpr std::size_t header_checksum_result = 0x14d;

namespace {

std::string_view destination(const byte b) {
  switch(b) {
  case 0x00: return "JP";
  case 0x01: return "Other";
  default: return "Unknown";
  }
}

std::string_view sgb_support(const byte b) {
  switch(b) {
  case 0x00: return "compatible";
  case 0x03: return "need sgb functionalities";
  default: return "";
  }
}

std::string_view cgb_support(const byte b) {
  switch(b) {
  case 0x00: return "dmg-only";
  case 0x80: return "dmg-compatible";
  case 0xc0: return "cgb-only";
  default: return "";
  }
}

std::string_view mbc(const byte b) {
  switch(b) {
  case 0x00: return "rom_only";
  case 0x01: return "mbc1";
  case 0x02: return "mbc1+ram";
  case 0x03: return "mbc1+ram+battery";
  case 0x05: return "mbc2";
  case 0x06: return "mbc2+battery";
  case 0x08: return "rom+ram";
  case 0x09: return "rom+ram+battery";
  case 0x0B: return "mmm01";
  case 0x0C: return "mmm01+ram";
  case 0x0D: return "mmm01+ram+battery";
  case 0x0F: return "mbc3+timer+battery";
  case 0x10: return "mbc3+timer+ram+battery";
  case 0x11: return "mbc3";
  case 0x12: return "mbc3+ram";
  case 0x13: return "mbc3+ram+battery";
  case 0x19: return "mbc5";
  case 0x1A: return "mbc5+ram";
  case 0x1B: return "mbc5+ram+battery";
  case 0x1C: return "mbc5+rumble";
  case 0x1D: return "mbc5+rumble+ram";
  case 0x1E: return "mbc5+rumble+ram+battery";
  case 0x20: return "mbc6";
  case 0x22: return "mbc7+sensor+rumble+ram+battery";
  case 0xFC: return "pocket camera";
  case 0xFD: return "bandai tama5";
  case 0xFE: return "HuC3";
  case 0xFF: return "HuC1+ram+battery";
  default: return "";
  }
}

std::string_view rom(const byte b) {
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
  default: return "";
  }
}

std::string_view ram(const byte b) {
  switch(b) {
  case 0x00: return "0_kb";
  case 0x01: return "-";
  case 0x02: return "8_kb";
  case 0x03: return "32_kb";
  case 0x04: return "128_kb";
  case 0x05: return "64_kb";
  default: return "";
  }
}

std::string_view new_licensee(const byte a, const byte c) {
  const char str[3]{static_cast<char>(a), static_cast<char>(c), '\0'};
  const std::string_view sv{str};

  if(sv == "00") return "None";
  else if(sv == "01") return "Nintendo";
  else if(sv == "08") return "Capcom";
  else if(sv == "13") return "Electronic arts";
  else if(sv == "18") return "Hudsonsoft";
  else if(sv == "19") return "B-ai";
  else if(sv == "20") return "Kss";
  else if(sv == "22") return "Pow";
  else if(sv == "24") return "Pcm complete";
  else if(sv == "25") return "San-x";
  else if(sv == "28") return "Kemco japan";
  else if(sv == "29") return "Seta";
  else if(sv == "30") return "Viacom";
  else if(sv == "31") return "Nintendo";
  else if(sv == "32") return "Bandia";
  else if(sv == "33") return "Ocean/acclaim";
  else if(sv == "34") return "Konami";
  else if(sv == "35") return "Hector";
  else if(sv == "37") return "Taito";
  else if(sv == "38") return "Hudson";
  else if(sv == "39") return "Banpresto";
  else if(sv == "41") return "Ubi soft";
  else if(sv == "42") return "Atlus";
  else if(sv == "44") return "Malibu";
  else if(sv == "46") return "Angel";
  else if(sv == "47") return "Pullet-proof";
  else if(sv == "49") return "Irem";
  else if(sv == "50") return "Absolute";
  else if(sv == "51") return "Acclaim";
  else if(sv == "52") return "Activision";
  else if(sv == "53") return "American sammy";
  else if(sv == "54") return "Konami";
  else if(sv == "55") return "Hi tech entertainment";
  else if(sv == "56") return "Ljn";
  else if(sv == "57") return "Matchbox";
  else if(sv == "58") return "Mattel";
  else if(sv == "59") return "Milton bradley";
  else if(sv == "60") return "Titus";
  else if(sv == "61") return "Virgin";
  else if(sv == "64") return "Lucasarts";
  else if(sv == "67") return "Ocean";
  else if(sv == "69") return "Electronic arts";
  else if(sv == "70") return "Infogrames";
  else if(sv == "71") return "Interplay";
  else if(sv == "72") return "Broderbund";
  else if(sv == "73") return "Sculptured";
  else if(sv == "75") return "Sci";
  else if(sv == "78") return "T*hq";
  else if(sv == "79") return "Accolade";
  else if(sv == "80") return "Misawa";
  else if(sv == "83") return "Lozc";
  else if(sv == "86") return "Tokuma shoten i*";
  else if(sv == "87") return "Tsukuda original";
  else if(sv == "91") return "Chun soft";
  else if(sv == "92") return "Video system";
  else if(sv == "93") return "Ocean/acclaim";
  else if(sv == "95") return "Varie";
  else if(sv == "96") return "Yonezawa/s'pal";
  else if(sv == "97") return "Kaneko";
  else if(sv == "99") return "Pack in soft";
  else if(sv == "A4") return "Konami (Yu-Gi-Oh!)";
  else return "Unknown";
}

std::string_view licensee(byte b, [[maybe_unused]] byte c, [[maybe_unused]] byte d) {
  if(b == 0x33) return new_licensee(c, d);

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
  default: return "";
  }
}
}

int main(int argc, const char *const argv[]) {
  if(argc < 2) return 1;

  std::ofstream fout{"cart.data", std::ios_base::app /*append*/};
  std::ifstream fin;

  for(const auto &e : std::filesystem::directory_iterator{argv[1]}) {
    if(!e.path().string().ends_with(".gb")) continue;

    fin.open(e.path());

    std::array<byte, cartridge_header_end> dump;
    fin.read((char *)dump.data(), cartridge_header_end); // read till cartridge header end
    fin.close();

    fmt::print(fout,
               "Name                : {}\n"                                            //
               "Title               : {}\n"                                            //
               "Nintendo logo check : {}\n"                                            //
               "Physical size       : {}\n"                                            //
               "ROM                 : {}\n"                                            //
               "RAM                 : {}\n"                                            //
               "MBC                 : {}\n"                                            //
               "Destination         : {}\n"                                            //
               "Color GB support    : {}\n"                                            //
               "Super GB support    : {}\n"                                            //
               "Publisher           : {}\n"                                            //
               "Version             : {}\n"                                            //
               "Header checksum     : {}\n\n",                                         //
               e.path().filename().string(),                                           //
               std::string_view(&(char &)dump[title_begin], &(char &)dump[title_end]), //
               std::equal(&dump[nintendo_logo_begin], &dump[nintendo_logo_end], nintendo_logo.begin())
                   ? "Passed"
                   : "Failed",
               e.file_size(),
               rom(dump[rom_size]),                      //
               ram(dump[ram_size]),                      //
               mbc(dump[mbc_type]),                      //
               destination(dump[destination_code]),      //
               cgb_support(dump[color_gameboy_support]), //
               sgb_support(dump[super_gameboy_support]), //
               licensee(dump[old_licensee_code], dump[new_licensee_code0], dump[new_licensee_code1]),
               dump[game_version],
               std::accumulate(&dump[header_checksum_begin], &dump[header_checksum_end], 0,
                               [](byte x, byte y) { return x - y - 1; }) == dump[header_checksum_result]
                   ? "Passed"
                   : "Failed");
  }

  return 0;
}
