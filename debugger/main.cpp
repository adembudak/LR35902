#include <LR35902/debugView/debugView.h>
#include <backend/Emu.h>

#include "Debugger.h"

int main(int argc, const char *const argv[]) {
  auto an_app = std::make_unique<Debugger>();
  an_app->run(std::move(an_app));
  return 0;
}
