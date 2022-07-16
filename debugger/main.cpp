#include <LR35902/core/core.h>
#include <LR35902/debugView/debugView.h>
#include <LR35902/stubs/builtin/builtin.h>
#include <LR35902/stubs/bus/bus.h>
#include <LR35902/stubs/cartridge/cartridge.h>
#include <LR35902/stubs/interrupt/interrupt.h>
#include <LR35902/stubs/io/io.h>
#include <LR35902/stubs/ppu/ppu.h>

#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>
#include <imgui/imgui.h>

#include <cstdio>
#include <string_view>

int main(int argc, char **argv) {
  using namespace LR35902;

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

        ImGui::GetIO().Fonts->AddFontFromFileTTF("../misc/font/source-code-pro/TTF/SourceCodePro-Regular.ttf",
                                                 14.0f);
        ImGui::GetIO().Fonts->Build();

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
        ImGui_ImplSDLRenderer_Init(m_renderer);

        Cartridge cart;
        cart.load(sv.data());

        PPU ppu;
        BuiltIn builtIn;
        IO io;
        Interrupt intr;

        Bus bus{cart, ppu, builtIn, io, intr};
        Core cpu{bus};

        DebugView debugView{cpu, io, cart, ppu, builtIn};

        bool done = false;
        while(!done) {
          cpu.run();

          SDL_Event event;
          while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) done = true;
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
               event.window.windowID == SDL_GetWindowID(m_window))
              done = true;
          }

          ImGui_ImplSDLRenderer_NewFrame();
          ImGui_ImplSDL2_NewFrame();
          ImGui::NewFrame();

          if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
              if(ImGui::MenuItem("Open", "Ctrl-O")) {
              }

              if(ImGui::MenuItem("Close", "Alt-<F4>")) {
              }

              ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("View")) {

              if(ImGui::BeginMenu("Memory chunks", debugView._memory_portions)) {

                ImGui::MenuItem("ROM", NULL, &debugView._memory_portions_rom);
                ImGui::MenuItem("VRAM", NULL, &debugView._memory_portions_vram);
                ImGui::MenuItem("SRAM", NULL, &debugView._memory_portions_sram, cart.hasSRAM());
                ImGui::MenuItem("WRAM", NULL, &debugView._memory_portions_wram);
                ImGui::MenuItem("echo", NULL, &debugView._memory_portions_echo);
                ImGui::MenuItem("OAM", NULL, &debugView._memory_portions_oam);
                ImGui::MenuItem("No usable area", NULL, &debugView._memory_portions_noUsable);
                ImGui::MenuItem("IO", NULL, &debugView._memory_portions_io);
                ImGui::MenuItem("HRAM", NULL, &debugView._memory_portions_hram);

                ImGui::EndMenu();
              }

              ImGui::MenuItem("Disassembly", NULL, &debugView._disassembly);
              ImGui::MenuItem("CPU State", NULL, &debugView._cpu_state);
              ImGui::MenuItem("Registers", NULL, &debugView._registers);
              ImGui::MenuItem("Cartridge header", NULL, &debugView._cartridge_header);

              ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
          }

          debugView.showDisassembly();
          debugView.showCartridgeHeader();
          debugView.showMemoryPortions();
          debugView.showCPUState();
          debugView.showRegisters();

          ImGui::Render();
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
