#include "../GameBoy.h"
#include <LR35902/debugView/debugView.h>

#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

#include <CLI/CLI.hpp>

#include <fmt/core.h>

#include <chrono>
#include <filesystem>
#include <ratio>
#include <string_view>
#include <thread>

/*
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
*/

int main(int argc, char *argv[]) {
  CLI::App debugger{"LR35902 debugger", "debugger_sfml"};

  bool skipboot;
  std::string romFile;
  debugger.add_flag("-s,--skipboot", skipboot, "Skip boot");
  debugger.add_option("[rom].gb", romFile)->required()->check(CLI::ExistingFile);

  CLI11_PARSE(debugger, argc, argv);

  GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  if(skipboot) attaboy.skipboot();
  else attaboy.skipboot(false);

  attaboy.plug(romFile);

  sf::RenderWindow window{sf::VideoMode(1280, 720), "LR35902 debugger"};
  window.setFramerateLimit(60);

  ImGui::SFML::Init(window);

  // sf::CircleShape shape(100.f);
  // shape.setFillColor(sf::Color::Green);

  sf::Clock deltaClock;
  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);

      // clang-format off
      switch(event.type) {
      case sf::Event::Closed: attaboy.stop(); break;

      case sf::Event::KeyPressed:
        switch(event.key.code) {
        case sf::Keyboard::A:      attaboy.joypad.update(lr::button::a,      lr::keystatus::pressed); break;
        case sf::Keyboard::B:      attaboy.joypad.update(lr::button::b,      lr::keystatus::pressed); break;
        case sf::Keyboard::Return: attaboy.joypad.update(lr::button::select, lr::keystatus::pressed); break;
        case sf::Keyboard::Space:  attaboy.joypad.update(lr::button::start,  lr::keystatus::pressed); break;

        case sf::Keyboard::Up:     attaboy.joypad.update(lr::button::up,     lr::keystatus::pressed); break;
        case sf::Keyboard::Right:  attaboy.joypad.update(lr::button::right,  lr::keystatus::pressed); break;
        case sf::Keyboard::Down:   attaboy.joypad.update(lr::button::down,   lr::keystatus::pressed); break;
        case sf::Keyboard::Left:   attaboy.joypad.update(lr::button::left,   lr::keystatus::pressed); break;
        }
        break;

      case sf::Event::KeyReleased:
        switch(event.key.code) {
        case sf::Keyboard::A:      attaboy.joypad.update(lr::button::a,      lr::keystatus::released); break;
        case sf::Keyboard::B:      attaboy.joypad.update(lr::button::b,      lr::keystatus::released); break;
        case sf::Keyboard::Return: attaboy.joypad.update(lr::button::select, lr::keystatus::released); break;
        case sf::Keyboard::Space:  attaboy.joypad.update(lr::button::start,  lr::keystatus::released); break;

        case sf::Keyboard::Up:     attaboy.joypad.update(lr::button::up,     lr::keystatus::released); break;
        case sf::Keyboard::Right:  attaboy.joypad.update(lr::button::right,  lr::keystatus::released); break;
        case sf::Keyboard::Down:   attaboy.joypad.update(lr::button::down,   lr::keystatus::released); break;
        case sf::Keyboard::Left:   attaboy.joypad.update(lr::button::left,   lr::keystatus::released); break;
        }
        break;
      }
      // clang-format on
    }

    attaboy.update();

    ImGui::SFML::Update(window, deltaClock.restart());

    ImGui::Begin("Hello, world!");

    static int i = 0;
    if(ImGui::Button("Button")) {
      ++i;
    }

    ImGui::Text("%d ", i);

    ImGui::End();

    window.clear();
    //  window.draw(shape);
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}

/*

  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();

  if(fs::path font_path{"../misc/font"}; fs::exists(font_path)) {
    font_path /= "source-code-pro/TTF/SourceCodePro-Regular.ttf";
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), 14.0f);
    io.Fonts->Build();
  }

  bool show_bg = true;
  bool show_win = true;
  bool show_obj = true;

  while(attaboy.isPowerOn()) {
    SDL_Event event;

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
*/
