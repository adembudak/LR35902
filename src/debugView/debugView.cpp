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

void DebugView::show(const Core &c) const noexcept {
  registerStatus(c);
}

void DebugView::registerStatus(const Core &m_core) const noexcept {
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Registers:");
  // clang-format off
    ImGui::Text("A: %u", m_core.A.data()); ImGui::SameLine(); ImGui::Text("F: %u", m_core.F.data());
    ImGui::Text("B: %u", m_core.B.data()); ImGui::SameLine(); ImGui::Text("C: %u", m_core.C.data()); // m_core.BC.data();
    ImGui::Text("D: %u", m_core.D.data()); ImGui::SameLine(); ImGui::Text("E: %u", m_core.E.data()); // m_core.DE.data();
    ImGui::Text("H: %u", m_core.H.data()); ImGui::SameLine(); ImGui::Text("L: %u", m_core.L.data()); // m_core.HL.data();
    ImGui::Text("SP: %u", m_core.SP.m_data); ImGui::Text("PC: %u", m_core.PC.m_data);

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  // clang-format on
  ImGui::End();

  ImGui::Render();
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(m_renderer);
}

}
