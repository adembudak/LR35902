#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

#include <SDL2/SDL.h>
#include <imgui_impl_sdl.h>

namespace LR35902 {

class cartridge {
public:
  [[nodiscard]] cartridge(const std::filesystem::path p) {
    std::fstream f{p};                                    //
    m_data.assign(std::istreambuf_iterator<char>{f}, {}); //
  }

private:
  std::vector<byte> m_data;
};

}

int main(/*int argc, char **argv*/) {
  using namespace LR35902;

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
}
