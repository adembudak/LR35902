#include "../GameBoy.h"
#include <LR35902/debugView/debugView.h>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>
#include <ratio>
#include <string_view>
#include <thread>

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

  GameBoy attaboy;
  LR35902::DebugView debugView{attaboy};

  if(skipboot) attaboy.skipboot();
  else attaboy.skipboot(false);

  attaboy.plug(romFile);

  sf::RenderWindow window{sf::VideoMode(1280, 720), "LR35902 debugger"};

  ImGui::SFML::Init(window);

  sf::Texture texture;
  texture.create(160, 144);

  std::array<sf::Uint8, 160 * 144 * 4> pixels;

  sf::Clock deltaClock;
  while(attaboy.isPowerOn()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);

      // clang-format off
      switch(event.type) {
      default: 
        break;

      case sf::Event::KeyPressed:
        switch(event.key.code) {
        default: break;
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
        default: break;
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

    attaboy.update();
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
    debugView.visualizeVRAM();

    const auto &framebuffer = attaboy.ppu.getFrameBuffer();
    for(int i = 0; auto e : framebuffer) {
      switch(e) {
      case 0:
        pixels[i++] = 107;
        pixels[i++] = 166;
        pixels[i++] = 74;
        pixels[i++] = 255;
        break;
      case 1:
        pixels[i++] = 67;
        pixels[i++] = 122;
        pixels[i++] = 99;
        pixels[i++] = 255;
        break;
      case 2:
        pixels[i++] = 37;
        pixels[i++] = 89;
        pixels[i++] = 85;
        pixels[i++] = 255;
        break;
      case 3:
        pixels[i++] = 18;
        pixels[i++] = 66;
        pixels[i++] = 76;
        pixels[i++] = 255;
        break;
      }
    }

    texture.update(pixels.data());
    sf::Sprite sprite(texture);

    sprite.setPosition(30, 30);

    window.draw(sprite);
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
