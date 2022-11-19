#pragma once

#include <LR35902/config.h>

#include <string>

namespace LR35902::header {

enum class mbc : std::uint8_t;

constexpr std::size_t cartridge_header_begin = 0x100;
constexpr std::size_t cartridge_header_end = 0x14f + 1;

constexpr std::size_t logo_begin = 0x104;
constexpr std::size_t logo_end = 0x133 + 1;
constexpr std::size_t title_begin = 0x134;
constexpr std::size_t title_end = 0x143 + 1;
constexpr std::size_t manufacturer_code_begin = 0x13f;
constexpr std::size_t manufacturer_code_end = 0x142 + 1;
constexpr std::size_t color_gameboy_support = 0x143;
constexpr std::size_t new_licensee_code0 = 0x144;
constexpr std::size_t new_licensee_code1 = 0x145;
constexpr std::size_t super_gameboy_support = 0x146;
constexpr std::size_t mbc_code = 0x147;
constexpr std::size_t rom_code = 0x148;
constexpr std::size_t ram_code = 0x149;
constexpr std::size_t destination_code = 0x14a;
constexpr std::size_t old_licensee_code = 0x14b;
constexpr std::size_t game_version = 0x14c;

constexpr std::size_t checksum_begin = 0x134;
constexpr std::size_t checksum_end = 0x14c + 1;
constexpr std::size_t checksum_result = 0x14d;

std::string destination_name(const byte b) noexcept;

std::string sgb_support(const byte b) noexcept;
std::string cgb_support(const byte b) noexcept;

std::string mbc_type(const byte b) noexcept;

std::size_t ram_size(const byte b) noexcept;

std::string rom_type(const byte b) noexcept;
std::string ram_type(const byte b) noexcept;

std::string new_licensee_name(const byte a, const byte c) noexcept;
std::string licensee_name(byte b, [[maybe_unused]] byte c, [[maybe_unused]] byte d) noexcept;

}
