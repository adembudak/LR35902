#include <backend/GameBoy.h>
#include <debugger/sdl2/palettes.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <LR35902/debugView/debugView.h>

#include <SDL2/SDL.h>
#if !SDL_VERSION_ATLEAST(2, 0, 17)
  #error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <CLI/CLI.hpp>

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>

static void putMenuBar(LR35902::DebugView &debugView, bool &isDone) {
  if(ImGui::BeginMenu("File")) {
    if(ImGui::MenuItem("Open", "Ctrl-O")) {
    }
    if(ImGui::MenuItem("Close", "Alt-<F4>")) {
      isDone = true;
    }
    ImGui::EndMenu();
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

    ImGui::MenuItem("Cart Header", NULL, &debugView._header);
    ImGui::MenuItem("Disassembly", NULL, &debugView._disassembly);
    ImGui::MenuItem("CPU State", NULL, &debugView._cpu_state);
    ImGui::MenuItem("Registers", NULL, &debugView._registers);

    ImGui::EndMenu();
  }
}

int main(int argc, char *argv[]) {
  CLI::App app{"LR35902", "debugger_sdl2"};

  std::string romFile;
  app.add_option("[rom].gb", romFile)->required()->check(CLI::ExistingFile);

  try {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError &e) {
    return app.exit(e);
  }

  GameBoy attaboy;
  const bool isCartridgePlugged = attaboy.plug(romFile);

  if(const bool ret = attaboy.tryBoot(); !ret) {
    attaboy.skipboot();
  }

  if(const int ret = SDL_InitSubSystem(SDL_INIT_VIDEO); ret != 0) {
    return ret;
  }

  constexpr int w_win = 1280;
  constexpr int h_win = 720;

  // clang-format off
  constexpr auto flags_window = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  std::unique_ptr<SDL_Window, decltype([](SDL_Window *w) { SDL_DestroyWindow(w); })> my_window{ //
    SDL_CreateWindow("LR35902 + Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_win, h_win, flags_window)};

  if (!my_window) {
      SDL_Log("%s\n", SDL_GetError());
      return 1;
  }

  constexpr auto flags_renderer = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  std::unique_ptr<SDL_Renderer, decltype([](SDL_Renderer *r) { SDL_DestroyRenderer(r); })> my_renderer{
    SDL_CreateRenderer(my_window.get(), -1, flags_renderer)};

  if (!my_renderer) {
      SDL_Log("%s\n", SDL_GetError());
      return 2;
  }
  constexpr auto flags_texture = SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET;
  std::unique_ptr<SDL_Texture, decltype([](SDL_Texture *t) { SDL_DestroyTexture(t); })> my_texture{
    SDL_CreateTexture(my_renderer.get(), SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, flags_texture, w_win, h_win)};
  // clang-format on

  if(!my_texture) {
    SDL_Log("%s\n", SDL_GetError());
    return 3;
  }

  LR35902::DebugView debugView{attaboy};
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(my_window.get(), my_renderer.get());
  ImGui_ImplSDLRenderer2_Init(my_renderer.get());

  constexpr int emu_w = 160;
  constexpr int emu_h = 144;
  constexpr SDL_Rect emuOutput{.x = 50, .y = 50, .w = emu_w, .h = emu_h};

  std::vector<SDL_Color> pixels(emu_w * emu_h);

  bool isDone = false;
  while(!isDone) {

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(ImGui::BeginMainMenuBar()) {
      putMenuBar(debugView, isDone);
      ImGui::EndMainMenuBar();
    }

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      // clang-format off
      switch(event.type) {
      case SDL_EventType::SDL_QUIT:
        isDone = true;
        attaboy.stop();
        break;

      case SDL_EventType::SDL_KEYDOWN: {
        switch(event.key.keysym.sym) {
        case SDL_KeyCode::SDLK_a:     attaboy.joypad.update(lr::button::a,      lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_d:     attaboy.joypad.update(lr::button::b,      lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_s:     attaboy.joypad.update(lr::button::select, lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_w:     attaboy.joypad.update(lr::button::start,  lr::keystatus::pressed); break;

        case SDL_KeyCode::SDLK_UP:    attaboy.joypad.update(lr::button::up,     lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_RIGHT: attaboy.joypad.update(lr::button::right,  lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_DOWN:  attaboy.joypad.update(lr::button::down,   lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_LEFT:  attaboy.joypad.update(lr::button::left,   lr::keystatus::pressed); break;
        }
      } 
        break;

      case SDL_EventType::SDL_KEYUP: {
        switch(event.key.keysym.sym) {
        case SDL_KeyCode::SDLK_a:     attaboy.joypad.update(lr::button::a,      lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_b:     attaboy.joypad.update(lr::button::b,      lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_s:     attaboy.joypad.update(lr::button::select, lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_w:     attaboy.joypad.update(lr::button::start,  lr::keystatus::released); break;

        case SDL_KeyCode::SDLK_UP:    attaboy.joypad.update(lr::button::up,     lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_RIGHT: attaboy.joypad.update(lr::button::right,  lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_DOWN:  attaboy.joypad.update(lr::button::down,   lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_LEFT:  attaboy.joypad.update(lr::button::left,   lr::keystatus::released); break;
        }
      } 
        break;
      }
      // clang-format on
    }

    while(attaboy.isPowerOn()) {

      attaboy.update();

      static palette_t palette;
      debugView.showCartHeader();
      debugView.showMemoryPortions();
      debugView.showDisassembly();
      debugView.showCPUState();
      debugView.showRegisters();

      ImGui::Begin("palette");
      static int selected = 0;
      ImGui::SetNextItemWidth(100.0f);

      const char *items[]{"Original", "Coco'Cola", "Galata"};
      ImGui::Combo("Palette", &selected, items, IM_ARRAYSIZE(items));

      switch(selected) {
      case 0: palette = original; break;
      case 1: palette = cococola; break;
      case 2: palette = galata; break;
      }
      ImGui::End();

      const auto &framebuffer = attaboy.ppu.getFrameBuffer();
      std::ranges::transform(framebuffer, pixels.begin(),
                             [&](auto e) { return SDL_Color{palette[e].r, palette[e].g, palette[e].b, palette[e].a}; });
    }

    ImGui::Render();

    SDL_UpdateTexture(my_texture.get(), &emuOutput, pixels.data(), emu_w * sizeof(SDL_Color));

    SDL_RenderClear(my_renderer.get());

    SDL_RenderCopy(my_renderer.get(), my_texture.get(), nullptr, nullptr);

    SDL_SetRenderDrawColor(my_renderer.get(), 0xff, 0xff, 0xff, 0xff);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), my_renderer.get());
    SDL_RenderPresent(my_renderer.get());
  }

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  return 0;
}
