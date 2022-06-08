#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

namespace LR35902 {

class Core;

class DebugView {
private:
  SDL_Window *m_window;
  SDL_Renderer *m_renderer;

public:
  DebugView();
  ~DebugView();

  void show(const Core &c) const noexcept;

private:
  void registerStatus(const Core &m_core) const noexcept;
  void romDump(const Core &m_core) const noexcept;
};

}
