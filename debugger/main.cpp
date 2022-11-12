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

namespace fs = std::filesystem;

void pollEvent(GameBoy &emu) {
  static SDL_Event event;
  while(SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    switch(event.type) {
    case SDL_EventType::SDL_QUIT: //
      emu.stop();
      break;

    case SDL_EventType::SDL_WINDOWEVENT:
      if(event.window.event == SDL_WINDOWEVENT_CLOSE) emu.stop();
      break;

    case SDL_EventType::SDL_KEYDOWN: {
      const Uint8 *const state = SDL_GetKeyboardState(NULL);
      if(state[SDL_SCANCODE_RETURN]) emu.joypad(button::select, keyStatus::pressed);
      if(state[SDL_SCANCODE_SPACE]) emu.joypad(button::start, keyStatus::pressed);
      if(state[SDL_SCANCODE_W]) emu.joypad(button::up, keyStatus::pressed);
      if(state[SDL_SCANCODE_D]) emu.joypad(button::right, keyStatus::pressed);
      if(state[SDL_SCANCODE_S]) emu.joypad(button::down, keyStatus::pressed);
      if(state[SDL_SCANCODE_A]) emu.joypad(button::left, keyStatus::pressed);
    } break;

    case SDL_EventType::SDL_KEYUP: {
      const Uint8 *const state = SDL_GetKeyboardState(NULL);
      if(state[SDL_SCANCODE_RETURN]) emu.joypad(button::select, keyStatus::released);
      if(state[SDL_SCANCODE_SPACE]) emu.joypad(button::start, keyStatus::released);
      if(state[SDL_SCANCODE_W]) emu.joypad(button::up, keyStatus::released);
      if(state[SDL_SCANCODE_D]) emu.joypad(button::right, keyStatus::released);
      if(state[SDL_SCANCODE_S]) emu.joypad(button::down, keyStatus::released);
      if(state[SDL_SCANCODE_A]) emu.joypad(button::left, keyStatus::released);
    } break;
    }
  }
}

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

    ImGui::MenuItem("Disassembly", NULL, &debugView._disassembly);
    ImGui::MenuItem("CPU State", NULL, &debugView._cpu_state);
    ImGui::MenuItem("Registers", NULL, &debugView._registers);
    ImGui::MenuItem("VRAM", NULL, &debugView._vram);

    ImGui::EndMenu();
  }
}

int main(int argc, char **argv) {

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

  if(fs::path font_path{"../misc/font"}; fs::exists(font_path)) {
    io.Fonts->AddFontFromFileTTF((font_path / "source-code-pro/TTF/SourceCodePro-Regular.ttf").c_str(),
                                 14.0f);
    io.Fonts->Build();
  }

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
  ImGui_ImplSDLRenderer_Init(m_renderer);

  GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  const SDL_Rect border{20, 20, 256, 256};

  auto cbk = [&](const GameBoy::screen_t &f) {
    SDL_UpdateTexture(m_texture, &border, f.data(), sizeof(LR35902::rgba32) * 256);
  };

  attaboy.setDrawCallback(cbk);

  if(skipboot) {
    attaboy.skipboot();
  } else {
    attaboy.skipboot(false);
  }

  attaboy.plug(argv[1]);

  using namespace std::chrono;
  using frames = duration<int, std::ratio<1, 60>>;
  auto nextFrame = system_clock::now() + frames{0};
  auto lastFrame = nextFrame - frames{1};

  while(attaboy.isPowerOn()) {
    pollEvent(attaboy);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    const SDL_Rect view{20, 20, 160, 144};
    if(ImGui::BeginMainMenuBar()) {
      putMenuBar(attaboy, debugView);
      ImGui::EndMainMenuBar();
    }

    attaboy.update();

    debugView.showMemoryPortions();

    debugView.showDisassembly();
    debugView.showCPUState();
    debugView.showRegisters();
    debugView.visualizeVRAM();

    ImGui::Render();
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
    SDL_SetRenderDrawColor(m_renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawRect(m_renderer, &border);
    SDL_RenderDrawRect(m_renderer, &view);
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
