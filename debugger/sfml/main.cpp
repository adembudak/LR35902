#include "../GameBoy.h"
#include "palettes.h"

#if defined(WITH_DEBUGGER)
#include <LR35902/debugView/debugView.h>
#endif

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowBase.hpp>

#if defined(WITH_DEBUGGER)
#include <imgui-SFML.h>
#include <imgui.h>
#endif

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>
#include <ratio>
#include <string_view>
#include <thread>

#if defined(WITH_DEBUGGER)
void putMenuBar(GameBoy &gb, LR35902::DebugView &dv) {
  if(ImGui::BeginMenu("File")) {
    if(ImGui::MenuItem("Open", "Ctrl-O")) {
    }
    if(ImGui::MenuItem("Close", "Alt-<F4>")) {
      gb.stop();
    }
    ImGui::EndMenu();
  }

  if(ImGui::BeginMenu("View")) {

    if(ImGui::BeginMenu("Memory chunks", dv._memory_portions)) {

      ImGui::MenuItem("ROM", NULL, &dv._memory_portions_rom);
      ImGui::MenuItem("VRAM", NULL, &dv._memory_portions_vram);
      ImGui::MenuItem("SRAM", NULL, &dv._memory_portions_sram);
      ImGui::MenuItem("WRAM", NULL, &dv._memory_portions_wram);
      ImGui::MenuItem("echo", NULL, &dv._memory_portions_echo);
      ImGui::MenuItem("OAM", NULL, &dv._memory_portions_oam);
      ImGui::MenuItem("No usable area", NULL, &dv._memory_portions_noUsable);
      ImGui::MenuItem("IO", NULL, &dv._memory_portions_io);
      ImGui::MenuItem("HRAM", NULL, &dv._memory_portions_hram);

      ImGui::EndMenu();
    }

    ImGui::MenuItem("Cart Header", NULL, &dv._header);
    ImGui::MenuItem("Disassembly", NULL, &dv._disassembly);
    ImGui::MenuItem("CPU State", NULL, &dv._cpu_state);
    ImGui::MenuItem("Registers", NULL, &dv._registers);
    ImGui::MenuItem("VRAM", NULL, &dv._vram);

    ImGui::EndMenu();
  }
}
#endif

int main(int argc, char *argv[]) {
  // command line handling
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

  // emu
  GameBoy attaboy;

#if defined(WITH_DEBUGGER)
  LR35902::DebugView debugView{attaboy};
#endif

  if(skipboot) attaboy.skipboot();
  else attaboy.skipboot(false);

  attaboy.plug(romFile);

  sf::RenderWindow window{sf::VideoMode(1280, 720), "LR35902 debugger"};

#if defined(WITH_DEBUGGER)
  ImGui::SFML::Init(window);
#endif

  palette_t palette = original;

  sf::Texture texture;
  texture.create(160, 144);

  std::array<sf::Uint8, 160 * 144 * 4> pixels;

  // game loop
  sf::Clock deltaClock;
  while(attaboy.isPowerOn()) {

    // input loop
    sf::Event event;
    /*
    while(window.pollEvent(event)) {
#if defined(WITH_DEBUGGER)
      ImGui::SFML::ProcessEvent(window, event);
#endif

      // clang-format off
      switch(event.type) {
      default:
        break;

      case sf::Event::KeyPressed:
        switch(event.key.code) {
        default: break;
        case sf::Keyboard::A:      attaboy.joypad.update(lr::button::a,      lr::keystatus::pressed); break;
        case sf::Keyboard::D:      attaboy.joypad.update(lr::button::b,      lr::keystatus::pressed); break;
        case sf::Keyboard::S:      attaboy.joypad.update(lr::button::select, lr::keystatus::pressed); break;
        case sf::Keyboard::W:      attaboy.joypad.update(lr::button::start,  lr::keystatus::pressed); break;

        case sf::Keyboard::Up:     attaboy.joypad.update(lr::button::up,     lr::keystatus::pressed); break;
        case sf::Keyboard::Right:  attaboy.joypad.update(lr::button::right,  lr::keystatus::pressed); break;
        case sf::Keyboard::Down:   attaboy.joypad.update(lr::button::down,   lr::keystatus::pressed); break;
        case sf::Keyboard::Left:   attaboy.joypad.update(lr::button::left,   lr::keystatus::pressed); break;
        }
        break;

      case sf::Event::KeyReleased:
        switch(event.key.code) {
        default: break;
        case sf::Keyboard::A:      attaboy.joypad.update(lr::button::a,      lr::keystatus::released); break;
        case sf::Keyboard::D:      attaboy.joypad.update(lr::button::b,      lr::keystatus::released); break;
        case sf::Keyboard::S:      attaboy.joypad.update(lr::button::select, lr::keystatus::released); break;
        case sf::Keyboard::W:      attaboy.joypad.update(lr::button::start,  lr::keystatus::released); break;

        case sf::Keyboard::Up:     attaboy.joypad.update(lr::button::up,     lr::keystatus::released); break;
        case sf::Keyboard::Right:  attaboy.joypad.update(lr::button::right,  lr::keystatus::released); break;
        case sf::Keyboard::Down:   attaboy.joypad.update(lr::button::down,   lr::keystatus::released); break;
        case sf::Keyboard::Left:   attaboy.joypad.update(lr::button::left,   lr::keystatus::released); break;
        }
        break;

      case sf::Event::LostFocus:
        attaboy.pause();
        break;

      case sf::Event::GainedFocus:
        attaboy.resume();
        break;

      case sf::Event::Closed:
        attaboy.stop();
        break;
      }
      // clang-format on
    }
    */

    attaboy.update();

#if defined(WITH_DEBUGGER)

    ImGui::SFML::Update(window, deltaClock.restart());

    if(ImGui::BeginMainMenuBar()) {
      putMenuBar(attaboy, debugView);
      ImGui::EndMainMenuBar();
    }

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
#endif

    texture.update(pixels.data());
    sf::Sprite sprite(texture);

    sprite.setPosition(30, 30);

    window.clear();
    window.draw(sprite);
#if defined(WITH_DEBUGGER)
    ImGui::SFML::Render(window);
#endif
    window.display();
  }

#if defined(WITH_DEBUGGER)
  ImGui::SFML::Shutdown();
#endif

  return 0;
}
