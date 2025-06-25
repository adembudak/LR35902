#pragma once

#include "AppBase.h"

#include <memory>

struct Emu;

namespace LR35902 {
struct DebugView;
}

class Debugger : public Application::AppBase {
private:
  std::shared_ptr<Emu> emulator;
  std::shared_ptr<LR35902::DebugView> debugview;

  GLuint programID;

  GLuint textureID;
  GLuint frameBufferObjectID;

  double lastTime = 0;

  bool show_output_window = 1;

  bool show_imgui_demo_window = 0;
  bool show_imgui_metrics_window = 1;

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
};
