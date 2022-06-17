#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

namespace LR35902 {

class Core;
class Cartridge;

class DebugView {
public:
  static void registerStatus(const Core &m_core) noexcept;
  static void romDump(const Cartridge &cart) noexcept;
};

}
