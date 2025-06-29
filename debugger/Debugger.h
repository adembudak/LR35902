#pragma once

#include "AppBase.h"

#include <ImGuiFileDialog.h>

#include <memory>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct Emu;

namespace LR35902 {
struct DebugView;
}

struct rgba8 {
  GLubyte r, g, b, a;
};

static constexpr std::array<rgba8, 4> pal{
    rgba8{107, 166, 74, 255},
    rgba8{67,  122, 99, 255},
    rgba8{37,  89,  85, 255},
    rgba8{18,  66,  76, 255}
};

class Debugger : public Application::AppBase {
public:
  enum class state_t : std::uint8_t { booting, seekingROM, running, stalled };

private:
  std::shared_ptr<Emu> emulator;
  std::shared_ptr<LR35902::DebugView> debugview;

  std::vector<std::string> romFiles;

  GLuint programID;

  GLuint textureID;
  GLuint frameBufferObjectID;

  double lastTime = 0;

  bool show_emulator_screen = 1;

  bool show_imgui_demo_window = 1;
  bool show_imgui_metrics_window = 1;

  ImGuiFileDialog fileSelector;

  state_t state;

private:
  void mainMenu();

public:
  virtual void init() override;
  virtual void startup() override;
  virtual void render(double currentTime) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
  virtual void onMouseMove(int x, int y) override;
  virtual void onPathDrop(const std::vector<std::string> &paths) override;
};
