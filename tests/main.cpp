#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"
#include "LR35902/stubs/bus/bus.h"
#include "LR35902/stubs/cartridge/cartridge.h"

#include <SDL2/SDL.h>
#include <imgui_impl_sdl.h>

#include <cstdio>
#include <string_view>

int main(int argc, char **argv) {
  using namespace LR35902;

  if(argc > 1) {
    if(std::string_view sv{argv[1]}; sv.ends_with(".gb")) {
      try {
        Cartridge cart;
        cart.load(sv.data());

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

          debugger.registerStatus(cpu);
        }
      }
      catch(const std::exception &e) {
        std::puts("Something gone wrong when opening ROM :(");
        std::puts(e.what());
      }
    } else {
      std::puts("Not a rom file!");
      return 1;
    }
  }

  return 0;
}
