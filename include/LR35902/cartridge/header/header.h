#pragma once

#include <LR35902/config.h>

#include <array>
#include <string>

namespace LR35902 {

enum class mbc : std::uint8_t;

constexpr std::size_t cartridge_header_begin = 0x100;
constexpr std::size_t cartridge_header_end = 0x14f + 1;

class header_t {
  std::array<byte, cartridge_header_end> m_data;

public:
  header_t() = default;
  header_t(std::array<byte, cartridge_header_end> n) :
      m_data(std::move(n)) {}

  void assign(std::array<byte, cartridge_header_end> &&t) {
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
