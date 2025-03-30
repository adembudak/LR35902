#include <LR35902/debugView/debugView.h>
#include <backend/GameBoy.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <CLI/CLI.hpp>

#include <algorithm>
#include <array>
#include <cstdint>

namespace {
using palette_t = std::array<SDL_Color, 4>;

constexpr palette_t original = {
    SDL_Color{107, 166, 74},
    SDL_Color{67,  122, 99},
    SDL_Color{37,  89,  85},
    SDL_Color{18,  66,  76}
};

constexpr palette_t cococola = {
    SDL_Color{244, 0, 9},
    SDL_Color{186, 0, 6},
    SDL_Color{114, 0, 4},
    SDL_Color{43,  0, 1}
};

constexpr palette_t galata = {
    SDL_Color{117, 148, 166},
    SDL_Color{101, 130, 144},
    SDL_Color{32,  46,  72 },
    SDL_Color{34,  63,  69 }
};

void putMenuBar(LR35902::DebugView &debugView, bool &isDone) {
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

  LR35902::DebugView debugView{attaboy};

  if(const int ret = SDL_InitSubSystem(SDL_INIT_VIDEO); ret != 0) {
    return ret;
  }

  const char *glsl_version = "#version 460";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  constexpr int win_w = 1280;
  constexpr int win_h = 720;

  constexpr auto flags_window = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  SDL_Window *my_window =
      SDL_CreateWindow("LR35902 + Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, flags_window);

  if(my_window == nullptr) {
    SDL_Log("%s\n", SDL_GetError());
    return 1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(my_window);

  if(gl_context == nullptr) {
    SDL_Log("%s\n", SDL_GetError());
    return 2;
  }

  SDL_GL_MakeCurrent(my_window, gl_context);
  SDL_GL_SetSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(my_window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  constexpr int emu_w = 160;
  constexpr int emu_h = 144;
  constexpr SDL_Rect emuOutput{.x = 50, .y = 50, .w = emu_w, .h = emu_h};

  std::vector<SDL_Color> pixels(emu_w * emu_h);

  bool isDone = false;
  while(!isDone) {

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      // clang-format off
      switch(event.type) {
      case SDL_EventType::SDL_QUIT:
        isDone = true;
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

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(ImGui::BeginMainMenuBar()) {
      putMenuBar(debugView, isDone);
      ImGui::EndMainMenuBar();
    }

    {
      ImGui::Begin("Debugger State");

      if(ImGui::Button("Reset")) {
        attaboy.reset();
      }

      if(ImGui::Button("Start")) {
        // attaboy.start();
      }

      if(ImGui::Button("Pause")) {
        attaboy.pause();
      }

      if(ImGui::Button("Resume")) {
        attaboy.resume();
      }

      if(ImGui::Button("Stop")) {
        attaboy.stop();
      }

      ImGui::End();
    }

    if(1) {

      attaboy.update();

      debugView.showCartHeader();
      debugView.showMemoryPortions();
      debugView.showDisassembly();
      debugView.showCPUState();
      debugView.showRegisters();

      const auto &framebuffer = attaboy.ppu.getFrameBuffer();
      std::ranges::transform(framebuffer, pixels.begin(), [&](const auto e) {
        return SDL_Color{original[e].r, original[e].g, original[e].b, original[e].a};
      });
    }

    static GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, emu_w, emu_h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<GLvoid *>(pixels.data()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    ImGui::Begin("emu");
    ImGui::Image(textureID, ImVec2{emu_w, emu_h});
    ImGui::End();

    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(my_window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(my_window);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  return 0;
}
