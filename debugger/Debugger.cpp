#include "Debugger.h"

#include <LR35902/debugView/debugView.h>
#include <backend/Emu.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ImGuiFileDialog.h>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace LR = LR35902;

void Debugger::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:

    if(mods & GLFW_MOD_CONTROL) {
      switch(key) {
      case GLFW_KEY_D: show_imgui_demo_window = !show_imgui_demo_window; break;
      case GLFW_KEY_M: show_imgui_metrics_window = !show_imgui_metrics_window; break;
      case GLFW_KEY_O: {
        IGFD::FileDialogConfig configuration;
        configuration.countSelectionMax = 1;
        configuration.path = ".";
        fileSelector.OpenDialog("ChooseFileDlgKey", "Choose File", ".gb,.gbc", configuration);

      } break;
      default: break;
      }
    }

    if(mods & GLFW_MOD_ALT) {
      switch(key) {
      case GLFW_KEY_F4: glfwWindowShouldClose(window); break;
      default:          break;
      }
    }

    if(key == GLFW_KEY_A) emulator->joypad.update(LR35902::button::a, LR35902::keystatus::pressed);
    if(key == GLFW_KEY_D) emulator->joypad.update(LR35902::button::b, LR35902::keystatus::pressed);

    if(key == GLFW_KEY_SPACE) emulator->joypad.update(LR35902::button::select, LR35902::keystatus::pressed);
    if(key == GLFW_KEY_ENTER) emulator->joypad.update(LR35902::button::start, LR35902::keystatus::pressed);

    if(key == GLFW_KEY_UP) emulator->joypad.update(LR35902::button::up, LR35902::keystatus::pressed);
    if(key == GLFW_KEY_RIGHT) emulator->joypad.update(LR35902::button::right, LR35902::keystatus::pressed);
    if(key == GLFW_KEY_DOWN) emulator->joypad.update(LR35902::button::down, LR35902::keystatus::pressed);
    if(key == GLFW_KEY_LEFT) emulator->joypad.update(LR35902::button::left, LR35902::keystatus::pressed);

    break;

  case GLFW_RELEASE:
    if(key == GLFW_KEY_A) emulator->joypad.update(LR35902::button::a, LR35902::keystatus::released);
    if(key == GLFW_KEY_D) emulator->joypad.update(LR35902::button::b, LR35902::keystatus::released);

    if(key == GLFW_KEY_SPACE) emulator->joypad.update(LR35902::button::select, LR35902::keystatus::released);
    if(key == GLFW_KEY_ENTER) emulator->joypad.update(LR35902::button::start, LR35902::keystatus::released);

    if(key == GLFW_KEY_UP) emulator->joypad.update(LR35902::button::up, LR35902::keystatus::released);
    if(key == GLFW_KEY_RIGHT) emulator->joypad.update(LR35902::button::right, LR35902::keystatus::released);
    if(key == GLFW_KEY_DOWN) emulator->joypad.update(LR35902::button::down, LR35902::keystatus::released);
    if(key == GLFW_KEY_LEFT) emulator->joypad.update(LR35902::button::left, LR35902::keystatus::released);
    break;

  case GLFW_REPEAT:
    switch(key) {
    default: break;
    }
    break;

  default: assert(false); break;
  }
}

void Debugger::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  break;
  case -1: break;
  default: break;
  }
}

void Debugger::onMouseMove(int x, int y) {
  float x_ = float(x) - (info.windowWidth / 2.0f);
  float y_ = float(y) - (info.windowHeight / 2.0f);
}

void Debugger::onPathDrop(const std::vector<std::string> &paths) {
  romFiles = paths;
}

void Debugger::mainMenu() {
  if(ImGui::BeginMainMenuBar()) {
    if(ImGui::BeginMenu("File")) {

      if(ImGui::MenuItem("Open ROM", "Ctrl-o")) {
        IGFD::FileDialogConfig configuration;
        configuration.countSelectionMax = 1;
        configuration.path = ".";
        fileSelector.OpenDialog("ChooseFileDlgKey", "Choose File", ".gb,.gbc", configuration);
      }

      if(ImGui::MenuItem("Close", "Alt+F4")) {
        glfwSetWindowShouldClose(AppBase::window, true);
      }

      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Output", nullptr, &show_emulator_screen);
      ImGui::MenuItem("Cart Header", nullptr, &debugview->_header);

      if(ImGui::BeginMenu("Memory Portions")) {
        ImGui::MenuItem("ROM", nullptr, &debugview->_memory_portions_rom);
        ImGui::MenuItem("VRAM", nullptr, &debugview->_memory_portions_vram);
        ImGui::MenuItem("SRAM", nullptr, &debugview->_memory_portions_sram);
        ImGui::MenuItem("WRAM", nullptr, &debugview->_memory_portions_wram);
        ImGui::MenuItem("Echo", nullptr, &debugview->_memory_portions_echo);
        ImGui::MenuItem("OAM", nullptr, &debugview->_memory_portions_oam);
        ImGui::MenuItem("Not Usable", nullptr, &debugview->_memory_portions_noUsable);
        ImGui::MenuItem("IO", nullptr, &debugview->_memory_portions_io);
        ImGui::MenuItem("HRAM", nullptr, &debugview->_memory_portions_hram);
        ImGui::EndMenu();
      }

      ImGui::MenuItem("Disassembly", nullptr, &debugview->_disassembly);
      ImGui::MenuItem("CPU State", nullptr, &debugview->_cpu_state);
      ImGui::MenuItem("Registers", nullptr, &debugview->_registers);
      ImGui::MenuItem("VRAM", nullptr, &debugview->_vram);

      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("ImGui")) {
      ImGui::MenuItem("Demo Window", "Ctrl-d", &show_imgui_demo_window);
      ImGui::MenuItem("Metrics/Debugger", "Ctrl-m", &show_imgui_metrics_window);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void Debugger::init() {
  info.title = "Debugger";
  AppBase::init();
}

void Debugger::startup() {
  emulator = std::make_shared<Emu>();
  debugview = std::make_shared<LR::DebugView>(*emulator);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  glCreateTextures(GL_TEXTURE_2D, 3, &textureID);
  glTextureStorage2D(textureID, 1, GL_RGBA8, LR::PPU::viewport_w, LR::PPU::viewport_h);
  glTextureSubImage2D(textureID, 0, 0, 0, LR::PPU::viewport_w, LR::PPU::viewport_h, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glCreateFramebuffers(1, &frameBufferObjectID);
  glNamedFramebufferTexture(frameBufferObjectID, GL_COLOR_ATTACHMENT0, textureID, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  assert(glGetError() == GL_NO_ERROR);

  state = state_t::booting;
}

void Debugger::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  mainMenu();

  if(show_imgui_demo_window) {
    ImGui::ShowDemoWindow(&show_imgui_demo_window);
  }

  if(show_imgui_metrics_window) {
    ImGui::ShowMetricsWindow(&show_imgui_metrics_window);
  }

  switch(state) {
  case state_t::booting: {
    bool bootROM_loaded = emulator->tryBoot();
    if(!bootROM_loaded) emulator->skipBoot();

    state = state_t::seekingROM;

  } break;

  case state_t::seekingROM: {

    for(const auto &rom : romFiles) {
      if(emulator->plug(rom)) {
        state = state_t::running;
        break;
      }
    }

    if(state != state_t::running) {
      emulator->reset();
      state = state_t::booting;
    }

  } break;

  case state_t::running: {
    emulator->update();

    if(debugview->_header) debugview->showCartHeader();
    if(debugview->_memory_portions) debugview->showMemoryPortions();
    if(debugview->_disassembly) debugview->showDisassembly();
    if(debugview->_cpu_state) debugview->showCPUState();
    if(debugview->_registers) debugview->showRegisters();
    if(debugview->_vram) debugview->showVRAM();

  } break;

  default: break;
  }

  if(fileSelector.Display("ChooseFileDlgKey")) {
    if(fileSelector.IsOk()) {
      romFiles.push_back(fileSelector.GetFilePathName());
    }

    fileSelector.Close();
  }

  if(show_emulator_screen) {

    const auto &framebuffer = emulator->ppu.getFrameBuffer();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjectID);

    std::array<rgba8, LR::PPU::viewport_h * LR::PPU::viewport_w> buf;

    ImGui::Begin("Screen", &show_emulator_screen);

    std::ranges::transform(framebuffer, buf.begin(), [&](const auto e) { return pal[e]; });

    glTextureSubImage2D(textureID, 0, 0, 0, LR::PPU::viewport_w, LR::PPU::viewport_h, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());

    ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2{160.0f, 144.0f});

    if(state != state_t::running) {
      ImGui::NewLine();
      ImGui::Text("Drag and drop a ROM file");
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Debugger::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}
