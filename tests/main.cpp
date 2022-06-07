#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

#include <SDL2/SDL.h>
#include <imgui_impl_sdl.h>

int main(int argc, char **argv) {
  using namespace LR35902;

  if(argc > 1) {
    Cartridge cart;
    cart.load(argv[1]);

    core cpu;

    debugView debugger;

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
    return 0;
  }

  return 1;
}
