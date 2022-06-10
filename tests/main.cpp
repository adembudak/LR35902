#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"
#include "LR35902/stubs/bus/bus.h"
#include "LR35902/stubs/cartridge/cartridge.h"

#include <SDL2/SDL.h>
#include <imgui_impl_sdl.h>

int main(int argc, char **argv) {
  using namespace LR35902;

  if(argc > 1) {
    Cartridge cart;
    cart.load(argv[1]);

    Bus bus{cart};
    Core cpu{bus};

    DebugView debugger;

    bool done = false;
    while(!done) {
      cpu.run();

      SDL_Event event;
      while(SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if(event.type == SDL_QUIT) done = true;
      }

      debugger.show(cpu);
    }
  }

  return 1;
}
