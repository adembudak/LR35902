#include <LR35902/core.h>

#include <cstdint>
#include <vector>

namespace LR35902 {

std::vector<byte> bus(1024);

void core::run() {

  /*
  #ifdef WITH_DEBUGGER
    print with imgui
  #endif

  auto fetchByte = [this] { return bus[PC]; };
  auto fetchWord = [this] { fetchByte() << 8 | fetchByte(); }

  switch (fetch()) {
  // generate body from: https://github.com/izik1/gbops/blob/master/dmgops.json
  }
  */
}

} // namespace LR35902
