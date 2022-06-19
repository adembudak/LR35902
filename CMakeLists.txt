cmake_minimum_required(VERSION 3.23)
project(LR3590 LANGUAGES CXX)

set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)

add_library(core src/core.cpp src/registers/r16.cpp src/registers/r8.cpp)

add_library(cartridge src/stubs/cartridge/cartridge.cpp)
target_include_directories(cartridge PRIVATE include)

add_library(bus src/stubs/bus/bus.cpp)
target_include_directories(bus PRIVATE include)

target_link_libraries(bus PRIVATE cartridge)

target_link_libraries(core PRIVATE bus)
target_include_directories(core PUBLIC ${LR3590_INCLUDE_DIR})

if(WITH_DEBUGGER)
  find_package(SDL2 REQUIRED)

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

  add_executable(test tests/main.cpp)
  target_link_libraries(test PUBLIC debugView)
  target_include_directories(test PUBLIC ${LR3590_INCLUDE_DIR})
endif()
