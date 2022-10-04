cmake_minimum_required(VERSION 3.24.1)

project(
  LR35902
  HOMEPAGE_URL https://github.com/adembudak/LR35902
  LANGUAGES CXX)

set(LR35902_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(LR35902_BINARY_DIR ${PROJECT_BINARY_DIR})
set(LR35902_INCLUDE_DIR ${LR35902_SOURCE_DIR}/include)
set(LR35902_TEST_DIR ${LR35902_SOURCE_DIR}/tests)

option(WITH_DEBUGGER "" OFF)
option(WITH_TOOLS "" OFF)
option(BUILD_TEST_ROMS "" OFF)
option(VISUALIZE_TARGETS "" OFF)
option(CACHE_BUILD "" OFF)
option(STATIC_ANALYSIS "" OFF)

find_package(range-v3 QUIET REQUIRED)

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
  src/io/io.cpp
  src/dma/dma.cpp
  src/timer/timer.cpp)

target_link_libraries(lr35902 PRIVATE range-v3::range-v3)
target_include_directories(lr35902 PUBLIC ${LR35902_INCLUDE_DIR})

if(WITH_DEBUGGER)
  find_package(SDL2 QUIET REQUIRED)
  find_package(fmt QUIET REQUIRED)

  target_compile_definitions(lr35902 PUBLIC WITH_DEBUGGER)

  add_library(debugView src/debugView/debugView.cpp)

  add_library(
    Dear_ImGui EXCLUDE_FROM_ALL
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/backends/imgui_impl_sdl.cpp
    3rdparty/imgui/backends/imgui_impl_sdlrenderer.cpp)

  target_include_directories(Dear_ImGui PUBLIC imgui)
  target_link_libraries(Dear_ImGui PRIVATE SDL2::SDL2 SDL2::SDL2-static)
  add_library(imgui::imgui ALIAS Dear_ImGui)

  add_library(imgui_memory_editor INTERFACE)
  target_include_directories(imgui_memory_editor INTERFACE 3rdparty/imgui_club)
  add_library(imgui::imgui_memory_editor ALIAS imgui_memory_editor)

  target_link_libraries(debugView PRIVATE imgui::imgui
                                          imgui::imgui_memory_editor lr35902)

  target_include_directories(debugView PUBLIC ${LR35902_INCLUDE_DIR} 3rdparty
                                              3rdparty/imgui_club)

  add_executable(debugger debugger/main.cpp debugger/GameBoy.cpp)
  target_link_libraries(debugger PRIVATE debugView fmt::fmt)
  target_include_directories(debugger PUBLIC ${LR35902_INCLUDE_DIR})
endif()

if(WITH_TOOLS)
  find_package(fmt QUIET REQUIRED)

  add_executable(header_dumper tools/header_dumper.cpp)
  target_link_libraries(header_dumper PRIVATE fmt::fmt)
endif()

if(BUILD_TEST_ROMS)
  find_program(RGBASM rgbasm REQUIRED)
  find_program(RGBLINK rgblink REQUIRED)
  find_program(RGBFIX rgbfix REQUIRED)

  function(add_test_rom)
    cmake_parse_arguments(test "" "OUTPUT_DIRECTORY;DIRECTORY;MBC_TYPE"
                          "COMPILE_FLAGS;LINK_FLAGS;FIX_FLAGS;SOURCE" ${ARGN})

    file(MAKE_DIRECTORY ${test_OUTPUT_DIRECTORY})

    foreach(e IN LISTS test_SOURCE)
      cmake_path(GET e STEM LAST_ONLY source)
      cmake_path(GET e PARENT_PATH source_dir)

      set(assembly_file ${source_dir}/${source}.asm)
      set(object_file ${test_OUTPUT_DIRECTORY}/${source}.o)
      set(rom_file ${test_OUTPUT_DIRECTORY}/${source}.gb)

      # cmake-format: off
      execute_process(COMMAND ${RGBASM} ${test_COMPILE_FLAGS} --output ${object_file} ${assembly_file})
      execute_process(COMMAND ${RGBLINK} ${test_LINK_FLAGS} --dmg --output ${rom_file} ${object_file})
      execute_process(COMMAND ${RGBFIX} --mbc-type ${test_MBC_TYPE} --title ${source} ${test_FIX_FLAGS} ${rom_file})
      execute_process(COMMAND ${CMAKE_COMMAND} -E rm ${object_file})
      # cmake-format: on
    endforeach()
  endfunction()

  file(GLOB instructions CONFIGURE_DEPENDS
       ${LR35902_TEST_DIR}/source/instructions/*.asm)

  add_test_rom(OUTPUT_DIRECTORY ${LR35902_TEST_DIR}/binary/instructions
               MBC_TYPE ROM_ONLY SOURCE ${instructions})
endif()

if(VISUALIZE_TARGETS)
  find_program(DOT dot REQUIRED)

  add_custom_target(
    graphviz ALL
    COMMAND ${CMAKE_COMMAND} --graphviz=tgt.dot ${LR35902_BINARY_DIR}
    COMMAND ${DOT} -Tpng tgt.dot -o targets.png
    WORKING_DIRECTORY ${LR35902_BINARY_DIR})
endif()

if(CACHE_BUILD)
  find_program(CCACHE ccache REQUIRED)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
endif()

if(STATIC_ANALYSIS)
  find_program(CLANG_TIDY NAMES clang-tidy-15 clang-tidy-14 clang-tidy-13
                                clang-tidy REQUIRED)
  find_program(CPPCHECK cppcheck REQUIRED)
endif()
