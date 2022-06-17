#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

namespace LR35902 {

class Core;
class Cartridge;

class DebugView {
public:
  void registerStatus(const Core &m_core) const noexcept;
  void romDump(const Cartridge &m_core) const noexcept;
};

}
