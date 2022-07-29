#include <algorithm> // for std::replace
#include <cstdint>   // for std::size_t
#include <filesystem>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>

using byte = std::uint8_t;

constexpr std::size_t title_begin = 0x134;
constexpr std::size_t title_end = 0x143 + 1;

constexpr std::size_t mbc_type = 0x147;
constexpr std::size_t rom_size = 0x148;
constexpr std::size_t ram_size = 0x149;

std::unordered_map<byte, std::string_view> mbc{
    {0x00, "rom_only"                      },
    {0x01, "mbc1"                          },
    {0x02, "mbc1+ram"                      },
    {0x03, "mbc1+ram+battery"              },
    {0x05, "mbc2"                          },
    {0x06, "mbc2+battery"                  },
    {0x08, "rom+ram"                       },
    {0x09, "rom+ram+battery"               },
    {0x0B, "mmm01"                         },
    {0x0C, "mmm01+ram"                     },
    {0x0D, "mmm01+ram+battery"             },
    {0x0F, "mbc3+timer+battery"            },
    {0x10, "mbc3+timer+ram+battery"        },
    {0x11, "mbc3"                          },
    {0x12, "mbc3+ram"                      },
    {0x13, "mbc3+ram+battery"              },
    {0x19, "mbc5"                          },
    {0x1A, "mbc5+ram"                      },
    {0x1B, "mbc5+ram+battery"              },
    {0x1C, "mbc5+rumble"                   },
    {0x1D, "mbc5+rumble+ram"               },
    {0x1E, "mbc5+rumble+ram+battery"       },
    {0x20, "mbc6"                          },
    {0x22, "mbc7+sensor+rumble+ram+battery"},
    {0xFC, "pocket camera"                 },
    {0xFD, "bandai tama5"                  },
    {0xFE, "HuC3"                          },
    {0xFF, "HuC1+ram+battery"              },
};

std::unordered_map<byte, std::string_view> rom{
    {0x00, "32_kb" },
    {0x01, "64_kb" },
    {0x02, "128_kb"},
    {0x03, "256_kb"},
    {0x04, "512_kb"},
    {0x05, "1_mb"  },
    {0x06, "2_mb"  },
    {0x07, "4_mb"  },
    {0x08, "8_mb"  },
    {0x52, "1.1_mb"},
    {0x53, "1.2_mb"},
    {0x54, "1.5_mb"}
};

std::unordered_map<byte, std::string_view> ram{
    {0x00, "0_kb"  },
    {0x01, "-"     },
    {0x02, "8_kb"  },
    {0x03, "32_kb" },
    {0x04, "128_kb"},
    {0x05, "64_kb" }
};

int main(int argc, const char *const argv[]) {
  if(argc < 2) return 1;

  namespace fs = std::filesystem;

  std::ofstream fout{"cart.data", std::ios_base::app};
  std::ifstream fin;

  fmt::print(fout, "{:<80} {:^30} {:^15} {:^15} {:^15}\n", "ROM_name", "mbc_type", "rom_physical_size",
             "rom_size", "ram_size");
  fmt::print(fout, "{}\n", std::string(140, '-'));

  for(const auto &e : fs::directory_iterator{argv[1]}) {
    if(!e.path().string().ends_with(".gb")) continue;

    fin.open(e.path());
    const std::vector<byte> dump(std::istreambuf_iterator<char>{fin}, {});
    fin.close();

    if(!dump.empty()) {
      std::string rom_name = e.path().filename().string();
      std::replace(rom_name.begin(), rom_name.end(), ' ', '_');

      fmt::print(fout, "{:<80} {:^30} {:^15} {:^15} {:^15}\n\n",
                 rom_name,            //
                 mbc[dump[mbc_type]], //
                 dump.size(),         //
                 rom[dump[rom_size]], //
                 ram[dump[ram_size]]);
    }
  }

  return 0;
}
