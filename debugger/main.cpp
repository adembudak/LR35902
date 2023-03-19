#include "GameBoy.h"
#include <LR35902/debugView/debugView.h>

#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>
#include <imgui/imgui.h>

#include <fmt/core.h>

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
    ImGui::MenuItem("VRAM", NULL, &debugView._vram);

    ImGui::EndMenu();
  }
}

int main(int argc, char *argv[]) {
  namespace fs = std::filesystem;
  namespace chrono = std::chrono;

  constexpr std::string_view help{"Usage: debugger [game.gb] [--skipboot, -s]\n"};

  if(argc < 2) {
    fmt::print("{}", help);
    return 1;
  }

  if(std::string_view sv{argv[1]}; sv == "-h" || sv == "--help") {
    fmt::print("{}", help);
    return 2;
  }

  if(std::string_view sv{argv[1]}; !sv.ends_with(".gb")) {
    fmt::print("Not a rom file!\n");
    return 3;
  }

  if(!fs::exists({argv[1]})) {
    fmt::print("No such file {} is found!\n", argv[1]);
    return 4;
  }

  bool skipboot{};
  if(argc == 3)
    if(std::string_view sv{argv[2]}; sv == "-s" || sv == "--skipboot") //
      skipboot = true;

  const auto w_win = 1280;
  const auto h_win = 720;

  const auto flags_win = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  SDL_Window *m_window = SDL_CreateWindow("LR35902 debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          w_win, h_win, flags_win);

  const auto flags_renderer = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  SDL_Renderer *m_renderer = SDL_CreateRenderer(m_window, -1, flags_renderer);

  const auto flags_texture = SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET;
  SDL_Texture *m_texture =
      SDL_CreateTexture(m_renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, flags_texture, w_win, h_win);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

 

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
  ImGui_ImplSDLRenderer_Init(m_renderer);

  GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  if(skipboot) attaboy.skipboot();
  else attaboy.skipboot(false);

  attaboy.plug(argv[1]);

  using frames = chrono::duration<int, std::ratio<1, 50>>;
  auto nextFrame = chrono::system_clock::now() + frames{0};
  auto lastFrame = nextFrame - frames{1};

  bool show_bg = true;
  bool show_win = true;
  bool show_obj = true;

  while(attaboy.isPowerOn()) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type) {
      case SDL_EventType::SDL_QUIT: attaboy.stop(); break;

      // clang-format off
      case SDL_EventType::SDL_KEYDOWN:
        if(!attaboy.joypad.isBlocked()) {
          switch(event.key.keysym.sym) {
          case SDL_KeyCode::SDLK_a:      attaboy.joypad.update(lr::button::a,      lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_b:      attaboy.joypad.update(lr::button::b,      lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_RETURN: attaboy.joypad.update(lr::button::select, lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_SPACE:  attaboy.joypad.update(lr::button::start,  lr::keystatus::pressed); break;

          case SDL_KeyCode::SDLK_UP:     attaboy.joypad.update(lr::button::up,     lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_RIGHT:  attaboy.joypad.update(lr::button::right,  lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_DOWN:   attaboy.joypad.update(lr::button::down,   lr::keystatus::pressed); break;
          case SDL_KeyCode::SDLK_LEFT:   attaboy.joypad.update(lr::button::left,   lr::keystatus::pressed); break;
          } break;
        }

      case SDL_EventType::SDL_KEYUP:
        if(!attaboy.joypad.isBlocked()) {
          switch(event.key.keysym.sym) {
          case SDL_KeyCode::SDLK_a:      attaboy.joypad.update(lr::button::a,      lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_b:      attaboy.joypad.update(lr::button::b,      lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_RETURN: attaboy.joypad.update(lr::button::select, lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_SPACE:  attaboy.joypad.update(lr::button::start,  lr::keystatus::released); break;

          case SDL_KeyCode::SDLK_UP:     attaboy.joypad.update(lr::button::up,     lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_RIGHT:  attaboy.joypad.update(lr::button::right,  lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_DOWN:   attaboy.joypad.update(lr::button::down,   lr::keystatus::released); break;
          case SDL_KeyCode::SDLK_LEFT:   attaboy.joypad.update(lr::button::left,   lr::keystatus::released); break;
          } break;
        }
        // clang-format on
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

    ImGui::Begin("Emu");
    ImGui::Checkbox("Background", &show_bg);
    ImGui::SameLine();
    ImGui::Checkbox("Window", &show_win);
    ImGui::SameLine();
    ImGui::Checkbox("Sprites", &show_obj);
    ImGui::NewLine();

    const auto &[bg, win, obj] = attaboy.ppu.GetFrameBuffer();
    ImDrawList *const drawlist = ImGui::GetWindowDrawList();
    const ImVec2 base = ImGui::GetCursorScreenPos();

    for(int y = 0; y < bg.size(); ++y) {
      for(int x = 0; x < bg[0].size(); ++x) {

        if(show_obj)
          drawlist->AddRectFilled(ImVec2{base.x + x, base.y + y}, ImVec2{base.x + x + 1, base.y + y + 1},
                                  ImColor{obj[y][x].r, obj[y][x].g, obj[y][x].b, obj[y][x].a});
        if(show_win)
          drawlist->AddRectFilled(ImVec2{base.x + x, base.y + y}, ImVec2{base.x + x + 1, base.y + y + 1},
                                  ImColor{win[y][x].r, win[y][x].g, win[y][x].b, win[y][x].a});
        if(show_bg)
          drawlist->AddRectFilled(ImVec2{base.x + x, base.y + y}, ImVec2{base.x + x + 1, base.y + y + 1},
                                  ImColor{bg[y][x].r, bg[y][x].g, bg[y][x].b, bg[y][x].a});
      }
    }

    ImGui::End();

    debugView.showCartHeader();
    debugView.showMemoryPortions();

    debugView.showDisassembly();
    debugView.showCPUState();
    debugView.showRegisters();
    debugView.visualizeVRAM();

    ImGui::Render();
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
    SDL_SetRenderDrawColor(m_renderer, 0xff, 0xff, 0xff, 0xff);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);

    std::this_thread::sleep_until(nextFrame);
    lastFrame = nextFrame;
    nextFrame += frames{1};
  }

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyTexture(m_texture);
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  SDL_Quit();

  return 0;
}
