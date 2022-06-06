#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

namespace LR35902 {

class core;

class debugView {
private:
  SDL_Window *m_window;
  SDL_Renderer *m_renderer;

public:
  debugView();
  ~debugView();

  void show(core &c);

private:
  void registerStatus(core &m_core);
  void romDump(core &m_core);
};

}
