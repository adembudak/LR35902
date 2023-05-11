#include "../GameBoy.h"
#include "palettes.h"

#include <LR35902/debugView/debugView.h>

#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer.h>

#include <fmt/core.h>

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>
#include <ratio>
#include <string_view>
#include <thread>

void putMenuBar(GameBoy &attaboy, LR35902::DebugView &debugView) {
  if(ImGui::BeginMenu("File")) {
    if(ImGui::MenuItem("Open", "Ctrl-O")) {
    }
    if(ImGui::MenuItem("Close", "Alt-<F4>")) {
      attaboy.stop();
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

  CLI::App debugger{"LR35902 debugger", "debugger_sfml"};

  bool skipboot;
  std::string romFile;
  debugger.add_flag("-s,--skipboot", skipboot, "Skip boot");
  debugger.add_option("[rom].gb", romFile)->required()->check(CLI::ExistingFile);

  try {
    debugger.parse(argc, argv);
  }
  catch(const CLI::ParseError &e) {
    return debugger.exit(e);
  }

  const auto w_win = 1280;
  const auto h_win = 720;

  const auto flags_win = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  SDL_Window *my_window = SDL_CreateWindow("LR35902 debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           w_win, h_win, flags_win);

  const auto flags_renderer = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  SDL_Renderer *my_renderer = SDL_CreateRenderer(my_window, -1, flags_renderer);

  const auto flags_texture = SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET;
  SDL_Texture *my_texture =
      SDL_CreateTexture(my_renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, flags_texture, w_win, h_win);

  std::array<Uint8, 160 * 144 * 4> pixels;
  palette_t palette = original;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();

  namespace fs = std::filesystem;
  if(fs::path font_path{"../../misc/font"}; fs::exists(font_path)) {
    font_path /= "source-code-pro/TTF/SourceCodePro-Regular.ttf";
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), 14.0f);
    io.Fonts->Build();
  }

  ImGui_ImplSDL2_InitForSDLRenderer(my_window, my_renderer);
  ImGui_ImplSDLRenderer_Init(my_renderer);

  GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  constexpr int emu_w = 160;
  constexpr int emu_h = 144;
  const SDL_Rect emuOutput{.x = 50, .y = 50, .w = emu_w, .h = emu_h};

  if(skipboot) attaboy.skipboot();
  else attaboy.skipboot(false);

  attaboy.plug(romFile.data());

  using namespace std::literals::chrono_literals;
  const auto frame_time = 58ms;

  while(attaboy.isPowerOn()) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type) {

      case SDL_EventType::SDL_QUIT:
        attaboy.stop();
        break;

        // clang-format off
      case SDL_EventType::SDL_KEYDOWN: {
        switch(event.key.keysym.sym) {
        case SDL_KeyCode::SDLK_a:      attaboy.joypad.update(lr::button::a,      lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_d:      attaboy.joypad.update(lr::button::b,      lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_s:      attaboy.joypad.update(lr::button::select, lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_w:      attaboy.joypad.update(lr::button::start,  lr::keystatus::pressed); break;

        case SDL_KeyCode::SDLK_UP:     attaboy.joypad.update(lr::button::up,     lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_RIGHT:  attaboy.joypad.update(lr::button::right,  lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_DOWN:   attaboy.joypad.update(lr::button::down,   lr::keystatus::pressed); break;
        case SDL_KeyCode::SDLK_LEFT:   attaboy.joypad.update(lr::button::left,   lr::keystatus::pressed); break;
        }
      } break;

      case SDL_EventType::SDL_KEYUP: {
        switch(event.key.keysym.sym) {
        case SDL_KeyCode::SDLK_a:      attaboy.joypad.update(lr::button::a,      lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_b:      attaboy.joypad.update(lr::button::b,      lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_s:      attaboy.joypad.update(lr::button::select, lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_w:      attaboy.joypad.update(lr::button::start,  lr::keystatus::released); break;

        case SDL_KeyCode::SDLK_UP:     attaboy.joypad.update(lr::button::up,     lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_RIGHT:  attaboy.joypad.update(lr::button::right,  lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_DOWN:   attaboy.joypad.update(lr::button::down,   lr::keystatus::released); break;
        case SDL_KeyCode::SDLK_LEFT:   attaboy.joypad.update(lr::button::left,   lr::keystatus::released); break;
        }
      } break;
      }
    }

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(ImGui::BeginMainMenuBar()) {
      putMenuBar(attaboy, debugView);
      ImGui::EndMainMenuBar();
    }

    attaboy.update();

    debugView.showCartHeader();
    debugView.showMemoryPortions();

    debugView.showDisassembly();
    debugView.showCPUState();
    debugView.showRegisters();

    // clang-format off
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
    // clang-format on

    const auto &framebuffer = attaboy.ppu.getFrameBuffer();
    for(int i = 0; auto e : framebuffer) {
      switch(e) {
      case 0:
        pixels[i++] = palette[0].r;
        pixels[i++] = palette[0].g;
        pixels[i++] = palette[0].b;
        pixels[i++] = palette[0].a;
        break;
      case 1:
        pixels[i++] = palette[1].r;
        pixels[i++] = palette[1].g;
        pixels[i++] = palette[1].b;
        pixels[i++] = palette[1].a;
        break;
      case 2:
        pixels[i++] = palette[2].r;
        pixels[i++] = palette[2].g;
        pixels[i++] = palette[2].b;
        pixels[i++] = palette[2].a;
        break;
      case 3:
        pixels[i++] = palette[3].r;
        pixels[i++] = palette[3].g;
        pixels[i++] = palette[3].b;
        pixels[i++] = palette[3].a;
        break;
      }
    }

    ImGui::Render();

    SDL_UpdateTexture(my_texture, &emuOutput, pixels.data(), emu_w * sizeof(SDL_Colour));

    SDL_RenderClear(my_renderer);

    SDL_RenderCopy(my_renderer, my_texture, nullptr, nullptr);

    SDL_SetRenderDrawColor(my_renderer, 0xff, 0xff, 0xff, 0xff);

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(my_renderer);

    const auto framerate =
        fmt::format("{:.3f} ms/frame ({:.3f} FPS)", 1.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    SDL_SetWindowTitle(my_window, framerate.c_str());
  }

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyTexture(my_texture);
  SDL_DestroyRenderer(my_renderer);
  SDL_DestroyWindow(my_window);
  SDL_Quit();

  return 0;
}
