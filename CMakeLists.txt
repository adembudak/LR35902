cmake_minimum_required(VERSION 3.23)
project(
  LR35902
  HOMEPAGE_URL https://github.com/adembudak/LR35902
  LANGUAGES CXX)

set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)

add_library(core src/core/core.cpp src/core/registers/r16.cpp
                 src/core/registers/r8.cpp)

add_library(
  stubs src/stubs/bus/bus.cpp src/stubs/cartridge/cartridge.cpp
        src/stubs/ppu/ppu.cpp src/stubs/builtin/builtin.cpp src/stubs/io/io.cpp)

target_include_directories(stubs PRIVATE include)

target_link_libraries(core PRIVATE stubs)
target_include_directories(core PUBLIC ${LR3590_INCLUDE_DIR})

if(WITH_DEBUGGER)
  find_package(SDL2 QUIET REQUIRED)

  add_library(
    imgui
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/backends/imgui_impl_sdl.cpp
    3rdparty/imgui/backends/imgui_impl_sdlrenderer.cpp)

  target_include_directories(imgui PUBLIC imgui imgui_club)
  target_link_libraries(imgui PUBLIC SDL2::SDL2 SDL2::SDL2-static)
  add_library(imgui::imgui ALIAS imgui)

  add_library(debugView src/debugView/debugView.cpp)
  target_link_libraries(debugView PUBLIC core imgui::imgui)
  target_include_directories(debugView PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty
                                              3rdparty/imgui_club)

  add_executable(debugger debugger/main.cpp)
  target_link_libraries(debugger PUBLIC debugView)
  target_include_directories(debugger PUBLIC ${LR3590_INCLUDE_DIR})
endif()
