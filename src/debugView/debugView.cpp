#include <LR35902/core.h>
#include <LR35902/debugView/debugView.h>

#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

namespace LR35902 {
/*
DebugView::DebugView() :
    m_window{SDL_CreateWindow("LR35902 debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)},
    m_renderer{SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED)} {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer); // setup renderer backends
  ImGui_ImplSDLRenderer_Init(m_renderer);
}

DebugView::~DebugView() {
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}
*/

void DebugView::registerStatus(const Core &m_core) const noexcept {
  using namespace ImGui;
  /*
ImGui_ImplSDLRenderer_NewFrame();
ImGui_ImplSDL2_NewFrame();
NewFrame();
*/

  Begin("Registers:");
  // clang-format off
    Text("A: %u", m_core.A.data()); SameLine(); Text("F: %u", m_core.F.data());
    Text("B: %u", m_core.B.data()); SameLine(); Text("C: %u", m_core.C.data()); Text("BC: %u", m_core.BC.data());
    Text("D: %u", m_core.D.data()); SameLine(); Text("E: %u", m_core.E.data()); Text("DE: %u", m_core.DE.data());
    Text("H: %u", m_core.H.data()); SameLine(); Text("L: %u", m_core.L.data()); Text("HL: %u", m_core.HL.data());

    Text("SP: %u", m_core.SP.m_data); Text("PC: %u", m_core.PC.m_data);

    Text("%.3f ms/frame (%.1f FPS)", 1/ ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  // clang-format on
  End();

  /*
  Render();
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(m_renderer);
  */
}

}
