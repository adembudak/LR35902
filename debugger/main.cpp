#include "GameBoy.h"
#include <LR35902/debugView/debugView.h>

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
  if(argc < 2) {
    std::puts("Usage: debugger [game.gb]");
    return 1;
  }

  if(std::string_view sv{argv[1]}; !sv.ends_with(".gb")) {
    std::puts("Not a rom file!");
    return 2;
  }

  SDL_Window *const m_window =
      SDL_CreateWindow("LR35902 debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  SDL_Renderer *const m_renderer =
      SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("../misc/font/source-code-pro/TTF/SourceCodePro-Regular.ttf", 14.0f);
  io.Fonts->Build();

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
  ImGui_ImplSDLRenderer_Init(m_renderer);

  LR35902::GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  attaboy.plug(argv[1]);
  attaboy.boot();

  while(attaboy.isPowerOn()) {

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if(event.type == SDL_QUIT) attaboy.stop();
      if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
         event.window.windowID == SDL_GetWindowID(m_window))
        attaboy.stop();
    }

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(ImGui::BeginMainMenuBar()) {

      // clang-format off
      if(ImGui::BeginMenu("File")) {
        if(ImGui::MenuItem("Open", "Ctrl-O")) {}
        if(ImGui::MenuItem("Close", "Alt-<F4>")) {}
        ImGui::EndMenu();
        // clang-format on
      }

      if(ImGui::BeginMenu("View")) {

        if(ImGui::BeginMenu("Memory chunks", debugView._memory_portions)) {

          ImGui::MenuItem("ROM", NULL, &debugView._memory_portions_rom);
          ImGui::MenuItem("VRAM", NULL, &debugView._memory_portions_vram);
          ImGui::MenuItem("SRAM", NULL, &debugView._memory_portions_sram);
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
        ImGui::MenuItem("VRAM", NULL, &debugView._vram);

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    //   constexpr std::size_t vblank_start_cycle = 16'416;
    //    for(std::size_t i = 0; i < vblank_start_cycle; i += attaboy.updateCycles()) {
    attaboy.play();
    //    }

    debugView.showDisassembly();
    debugView.showCartridgeHeader();
    debugView.showMemoryPortions();
    debugView.showCPUState();
    debugView.showRegisters();
    debugView.visualizeVRAM();

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

  return 0;
}
