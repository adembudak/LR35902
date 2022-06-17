#include "LR35902/core.h"
#include "LR35902/debugView/debugView.h"
#include "LR35902/stubs/bus/bus.h"
#include "LR35902/stubs/cartridge/cartridge.h"

#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <cstdio>
#include <string_view>

int main(int argc, char **argv) {
  using namespace LR35902;
  using namespace ImGui;

  if(argc > 1) {
    if(std::string_view sv{argv[1]}; sv.ends_with(".gb")) {
      try {
        SDL_Window *m_window =
            SDL_CreateWindow("LR35902 debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        SDL_Renderer *m_renderer =
            SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer); // setup renderer backends
        ImGui_ImplSDLRenderer_Init(m_renderer);

        Cartridge cart;
        cart.load(sv.data());

        Bus bus{cart};
        Core cpu{bus};

        DebugView debugger;

        bool done = false;
        while(!done) {
          cpu.run();

          ImGui_ImplSDLRenderer_NewFrame();
          ImGui_ImplSDL2_NewFrame();
          NewFrame();

          SDL_Event event;
          while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) done = true;
          }

          debugger.registerStatus(cpu);

          Render();
          SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
          SDL_RenderClear(m_renderer);
          ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
          SDL_RenderPresent(m_renderer);
        }

        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
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
