#include <LR35902/core.h>

#include <cstdint>
#include <vector>

#ifdef WITH_DEBUGGER
#endif

namespace LR35902 {

using byte = std::uint8_t;
std::vector<byte> bus(1024);

void core::run() {
  /*
auto fetch = [this] { return bus[PC]; };

switch (fetch()) {
// generate this
}
*/
}

} // namespace LR35902
