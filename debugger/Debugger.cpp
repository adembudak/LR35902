#include "Debugger.h"

#include <LR35902/debugView/debugView.h>
#include <backend/Emu.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <utility>

void Debugger::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE:
      AppBase::running = false;
      break;
      /*
      // Translate model
    case GLFW_KEY_K: my_model.translate({0.0, 0.1, 0.0}); break;
    case GLFW_KEY_L: my_model.translate({0.1, 0.0, 0.0}); break;
    case GLFW_KEY_J: my_model.translate({0.0, -0.1, 0.0}); break;
    case GLFW_KEY_H:
      my_model.translate({-0.1, 0.0, 0.0});
      break;

case GLFW_KEY_M: // Scale model
if(mods & GLFW_MOD_SHIFT) my_model.scale(glm::vec3{1.1, 1.1, 1.1});
else my_model.scale(glm::vec3{0.9, 0.9, 0.9});
break;

case GLFW_KEY_X: // Rotate model
if(mods & GLFW_MOD_SHIFT) my_model.rotate(-rotateAmount, {0.1, 0.0, 0.0});
else my_model.rotate(rotateAmount, {0.1, 0.0, 0.0});
break;

case GLFW_KEY_Y: my_model.rotate(rotateAmount, {0.0, 1.0, 0.0}); break;
case GLFW_KEY_Z: my_model.rotate(rotateAmount, {0.0, 0.0, 1.0}); break;

case GLFW_KEY_SPACE: Model::switchMeshMode(); break;
*/

    default: break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:
    switch(key) {
      /*
case GLFW_KEY_W: camera.moveAround(Camera::direction::front); break;
case GLFW_KEY_D: camera.moveAround(Camera::direction::right); break;
case GLFW_KEY_S: camera.moveAround(Camera::direction::back); break;
case GLFW_KEY_A: camera.moveAround(Camera::direction::left); break;
*/
    }
    break;

  default: break;
  }
}

void Debugger::onMouseWheel(int pos) {
  switch(pos) {
    //  case 1: camera.eye.z -= 1.0; break;
    //  case -1: camera.eye.z += 1.0; break;
  default: break;
  }
}

void Debugger::onMouseMove(int x, int y) {
  float x_ = float(x) - (info.windowWidth / 2.0f);
  float y_ = float(y) - (info.windowHeight / 2.0f);
  //  camera.lookAround(glm::radians(x_), glm::radians(y_), 0.0f);
}

void Debugger::mainMenu() {
  if(ImGui::BeginMainMenuBar()) {
    if(ImGui::BeginMenu("File")) {

      if(ImGui::MenuItem("Open ROM", "Ctrl+O")) {
      }

      if(ImGui::MenuItem("Close", "Alt+F4")) {
        this->running = false;
      }

      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Output", nullptr, &show_output_window);
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

      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("ImGui")) {
      ImGui::MenuItem("Demo Window", "d", &show_imgui_demo_window);
      ImGui::MenuItem("Metrics/Debugger", "m", &show_imgui_metrics_window);
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

  emulator->skipBoot();
  const std::string rom = "/home/adem/Github/LR35902/build/Debug/dmg-acid2.gb";
  auto _ = emulator->plug(rom);

  debugview = std::make_shared<LR35902::DebugView>(*emulator);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
  assert(glIsTexture(textureID));

  glTextureStorage2D(textureID, 1, GL_RGBA8, 160, 144);
  glTextureSubImage2D(textureID, 0, 0, 0, 160, 144, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void Debugger::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  mainMenu();

  if(show_imgui_demo_window) {
    ImGui::ShowDemoWindow(&show_imgui_demo_window);
  }

  if(show_imgui_metrics_window) {
    ImGui::ShowMetricsWindow(&show_imgui_metrics_window);
  }

  emulator->update();

  /*
  for(; emulator->ppu.mode() != LR35902::PPU::state::vblanking; emulator->step())
    ;

  constexpr int vblank_period_cycles = 1140;
  for(int i = 0; i < vblank_period_cycles; i += emulator->step())
    ;
    */

  debugview->showCartHeader();
  debugview->showMemoryPortions();
  debugview->showDisassembly();
  debugview->showCPUState();
  debugview->showRegisters();

  const auto &framebuffer = emulator->ppu.getFrameBuffer();

  struct color {
    GLubyte r, g, b, a;
  };

  constexpr std::array<color, 4> pal{
      color{107, 166, 74, 255},
      color{67,  122, 99, 255},
      color{37,  89,  85, 255},
      color{18,  66,  76, 255}
  };

  if(show_output_window) {
    static std::array<color, 160 * 144> buf;
    std::ranges::transform(framebuffer, buf.begin(), [&](const auto e) { return pal[e]; });

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTextureSubImage2D(textureID, 0, 0, 0, 160, 144, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    ImGui::Begin("Emu", &show_output_window);
    ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2{160.0f, 144.0f});
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
