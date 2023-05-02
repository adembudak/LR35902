#pragma once

#include <LR35902/config.h>
#include <LR35902/memory_map.h>

#include <array>
#include <string>

namespace LR35902 {

constexpr std::array<byte, mmap::logo_end - mmap::logo_begin> nintendo_logo{
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
    0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
    0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

class header_t {
  std::array<byte, mmap::header_end> m_data;

public:
  header_t() = default;
  header_t(std::array<byte, mmap::header_end> n) :
      m_data(std::move(n)) {}

  void assign(std::array<byte, mmap::header_end> &&t) {
    m_data = std::move(t);
  }

  [[nodiscard]] std::string decode_title() const noexcept;
  [[nodiscard]] std::string decode_destination_name() const noexcept;

  [[nodiscard]] std::string decode_sgb_support() const noexcept;
  [[nodiscard]] std::string decode_cgb_support() const noexcept;

  [[nodiscard]] bool is_logocheck_ok() const noexcept;
  [[nodiscard]] bool is_checksum_ok() const noexcept;

  [[nodiscard]] std::pair<std::string, byte> decode_mbc_type() const noexcept;

  [[nodiscard]] std::pair<std::string, std::size_t> decode_rom_size() const noexcept;
  [[nodiscard]] std::pair<std::string, std::size_t> decode_ram_size() const noexcept;

  [[nodiscard]] std::string decode_licensee_name() const noexcept;

  [[nodiscard]] std::size_t decode_version() const noexcept;
};

}
