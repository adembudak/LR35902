cmake_minimum_required(VERSION 3.23)

project(
  LR35902
  HOMEPAGE_URL https://github.com/adembudak/LR35902
  LANGUAGES CXX)

set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)
option(WITH_TOOLS "" OFF)
option(VISUALIZE_TARGETS "" OFF)

add_library(
  lr35902
  src/cpu/cpu.cpp
  src/cpu/registers/r16.cpp
  src/cpu/registers/r8.cpp
  src/bus/bus.cpp
  src/cartridge/cartridge.cpp
  src/cartridge/kind/rom_only.cpp
  src/cartridge/kind/rom_ram.cpp
  src/cartridge/kind/mbc1.cpp
  src/ppu/ppu.cpp
  src/builtin/builtin.cpp
  src/io/io.cpp)

target_include_directories(lr35902 PUBLIC ${LR3590_INCLUDE_DIR})

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
  target_link_libraries(imgui PRIVATE SDL2::SDL2 SDL2::SDL2-static)
  add_library(imgui::imgui ALIAS imgui)

  add_library(debugView src/debugView/debugView.cpp)
  target_link_libraries(debugView PRIVATE imgui::imgui lr35902)
  target_include_directories(debugView PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty
                                              3rdparty/imgui_club)

  target_compile_definitions(lr35902 PUBLIC WITH_DEBUGGER)

  add_executable(debugger debugger/main.cpp debugger/GameBoy.cpp)
  target_link_libraries(debugger PRIVATE debugView)
  target_include_directories(debugger PUBLIC ${LR3590_INCLUDE_DIR})
endif()

if(WITH_TOOLS)
  find_package(fmt QUIET REQUIRED CONFIG)

  add_executable(header_dumper tools/header_dumper.cpp)
  target_link_libraries(header_dumper PRIVATE fmt::fmt)
endif()

if(VISUALIZE_TARGETS)
  find_program(DOT dot REQUIRED)

  add_custom_target(
    graphviz ALL
    COMMAND ${CMAKE_COMMAND} --graphviz=tgt.dot ${PROJECT_BINARY_DIR}
    COMMAND dot -Tpng tgt.dot -o targets.png
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
