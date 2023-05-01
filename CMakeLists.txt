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
option(SFML_FRONTEND "" OFF)
option(SDL2_FRONTEND "" OFF)

option(WITH_TOOLS "" OFF)
option(ROM_TESTS "" OFF)
option(VISUALIZE_TARGETS "" OFF)
option(CACHE_BUILD "" OFF)
option(STATIC_ANALYSIS "" OFF)

find_package(range-v3 QUIET REQUIRED CONFIG)

add_library(
  lr35902
  src/cpu/cpu.cpp
  src/cpu/registers/r16.cpp
  src/cpu/registers/r8.cpp
  src/bus/bus.cpp
  src/cartridge/cartridge.cpp
  src/cartridge/header/header.cpp
  src/cartridge/kind/rom_only.cpp
  src/cartridge/kind/rom_ram.cpp
  src/cartridge/kind/mbc1_32kb.cpp
  src/cartridge/kind/mbc1_64kb.cpp
  src/cartridge/kind/mbc1_128kb.cpp
  src/cartridge/kind/mbc1_256kb.cpp
  src/cartridge/kind/mbc1_512kb.cpp
  src/cartridge/kind/mbc1_1mb.cpp
  src/cartridge/kind/mbc1_2mb.cpp
  src/cartridge/kind/mbc1_ram.cpp
  src/cartridge/kind/mbc2.cpp
  src/cartridge/kind/mbc5.cpp
  src/bootrom/bootrom.cpp
  src/ppu/ppu.cpp
  src/builtin/builtin.cpp
  src/io/io.cpp
  src/joypad/joypad.cpp
  src/dma/dma.cpp
  src/timer/timer.cpp
  src/interrupt/interrupt.cpp)

target_link_libraries(lr35902 PRIVATE range-v3::range-v3)
target_include_directories(lr35902 PUBLIC ${LR35902_INCLUDE_DIR})

if(WITH_DEBUGGER)
  find_package(imgui CONFIG REQUIRED)
  find_package(fmt QUIET REQUIRED CONFIG)

  add_library(imgui_memory_editor INTERFACE)
  target_include_directories(imgui_memory_editor INTERFACE ${LR35902_SOURCE_DIR}/3rdparty/imgui_club)
  add_library(imgui::imgui_memory_editor ALIAS imgui_memory_editor)

  add_library(debugView src/debugView/debugView.cpp)
  target_include_directories(debugView PUBLIC ${LR35902_INCLUDE_DIR} ${LR35902_SOURCE_DIR}/3rdparty/imgui_club)
  target_link_libraries(debugView PRIVATE imgui::imgui imgui::imgui_memory_editor lr35902)

  set_target_properties(lr35902 debugView PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
  if(SFML_FRONTEND)
    find_package(SFML QUIET REQUIRED COMPONENTS graphics CONFIG)
    find_package(ImGui-SFML QUIET REQUIRED CONFIG)

    add_executable(debugger_sfml debugger/sfml/main.cpp debugger/GameBoy.cpp)
    target_include_directories(debugger_sfml PUBLIC ${LR35902_INCLUDE_DIR})
    target_link_libraries(debugger_sfml PRIVATE debugView fmt::fmt imgui::imgui sfml-graphics ImGui-SFML::ImGui-SFML)

    set_target_properties(debugger_sfml PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
  endif()

  if(SDL2_FRONTEND)
    find_package(SDL2 QUIET REQUIRED CONFIG)

    add_executable(debugger_sdl debugger/sdl/main.cpp debugger/GameBoy.cpp)
    target_include_directories(debugger_sdl PUBLIC ${LR35902_INCLUDE_DIR})
    target_link_libraries(debugger_sdl PRIVATE debugView fmt::fmt imgui::imgui SDL2::SDL2-static SDL2::SDL2main)

    set_target_properties(debugger_sdl PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
  endif()

endif()

if(WITH_TOOLS)
  find_package(fmt QUIET REQUIRED)
  find_package(CLI11 QUIET REQUIRED)

  add_executable(gb.hd tools/header_dumper.cpp)
  target_link_libraries(gb.hd PRIVATE lr35902 fmt::fmt CLI11::CLI11)

  add_executable(gb.hf tools/header_fixer.cpp)
  target_link_libraries(gb.hf PRIVATE fmt::fmt CLI11::CLI11)
endif()

if(ROM_TESTS)
  find_program(RGBASM rgbasm REQUIRED)
  find_program(RGBLINK rgblink REQUIRED)
  find_program(RGBFIX rgbfix REQUIRED)

  function(add_test_rom)
    cmake_parse_arguments(test "" "OUTPUT_DIRECTORY;DIRECTORY;MBC_TYPE" "COMPILE_FLAGS;LINK_FLAGS;FIX_FLAGS;SOURCE"
                          ${ARGN})

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

  file(GLOB instructions CONFIGURE_DEPENDS ${LR35902_TEST_DIR}/source/instructions/*.asm)
  add_test_rom(OUTPUT_DIRECTORY ${LR35902_TEST_DIR}/binary/instructions MBC_TYPE ROM_ONLY SOURCE ${instructions})

  file(GLOB timer CONFIGURE_DEPENDS ${LR35902_TEST_DIR}/source/timer/*.asm)
  add_test_rom(OUTPUT_DIRECTORY ${LR35902_TEST_DIR}/binary/timer MBC_TYPE ROM_ONLY SOURCE ${timer})
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
  find_program(CLANG_TIDY NAMES clang-tidy-15 clang-tidy-14 clang-tidy-13 clang-tidy REQUIRED)
  find_program(CPPCHECK cppcheck REQUIRED)
endif()
