#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace LR35902 {

class cartridge {
public:
  /*
[[nodiscard]] cartridge(const char *rom) {
    std::fstream f{rom};                                  //
    m_data.assign(std::istreambuf_iterator<char>{f}, {}); //
}
*/

  [[nodiscard]] cartridge(const std::filesystem::path p) {
    std::fstream f{p};                                    //
    m_data.assign(std::istreambuf_iterator<char>{f}, {}); //
  }

private:
  std::vector<byte> m_data;
};

}

int main(int argc, char **argv) {
  using namespace LR35902;

  if(argc > 1) {
    cartridge cart{argv[1]};

    core cpu;
    debugView debugger;

    debugger.run(cpu);
  }

  return 0;
}
