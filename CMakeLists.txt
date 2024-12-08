cmake_minimum_required(VERSION 3.31.0)
project(
  LR35902
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/adembudak/LR35902
  LANGUAGES CXX)

include(CMakeDependentOption)
include(CTest)
include(FetchContent)

set(CMAKE_CXX_SCAN_FOR_MODULES 0)
set(CMAKE_CXX_EXTENSIONS 0)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_STANDARD 20)

set(LR35902_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(LR35902_BINARY_DIR ${PROJECT_BINARY_DIR})
set(LR35902_INCLUDE_DIR ${LR35902_SOURCE_DIR}/include)
set(LR35902_TEST_DIR ${LR35902_SOURCE_DIR}/tests)
set(LR35902_3RDPARTY_DIR ${LR35902_SOURCE_DIR}/3rdparty)

option(WITH_DEBUGGER "" OFF)
option(SFML_FRONTEND "" OFF)
option(SDL2_FRONTEND "" OFF)

option(WITH_TOOLS "" OFF)
cmake_dependent_option(tool_headerdumper "" OFF WITH_TOOLS ON)
cmake_dependent_option(tool_headerfixer "" OFF WITH_TOOLS ON)
cmake_dependent_option(tool_disassembler "" OFF WITH_TOOLS ON)

cmake_dependent_option(UNIT_TESTS "" OFF BUILD_TESTING OFF)
cmake_dependent_option(ROM_TESTS "" OFF BUILD_TESTING OFF)

option(VISUALIZE_TARGETS "" OFF)
option(CACHE_BUILD "" OFF)

option(STATIC_ANALYSIS "" OFF)
cmake_dependent_option(analyse_with_cppcheck "" OFF STATIC_ANALYSIS OFF)
cmake_dependent_option(analyse_with_clangtidy "" OFF STATIC_ANALYSIS OFF)
cmake_dependent_option(analyse_with_iwyu "" OFF STATIC_ANALYSIS OFF)
cmake_dependent_option(analyse_with_lwyu "" OFF STATIC_ANALYSIS OFF)

find_package(range-v3 QUIET REQUIRED CONFIG)
find_package(mpark_patterns QUIET REQUIRED CONFIG)

add_library(core)
target_sources(
  core
  PRIVATE src/cpu/cpu.cpp
          src/cpu/registers/r16.cpp
          src/cpu/registers/r8.cpp
          src/bus/bus.cpp
          src/cartridge/cartridge.cpp
          src/cartridge/header/header.cpp
          src/cartridge/kind/rom_only/rom_only.cpp
          src/cartridge/kind/rom_ram/rom_ram.cpp
          src/cartridge/kind/mbc1/mbc1.cpp
          src/cartridge/kind/mbc2/mbc2.cpp
          src/cartridge/kind/mbc3/mbc3.cpp
          src/cartridge/kind/mbc5/mbc5.cpp
          src/bootrom/bootrom.cpp
          src/ppu/ppu.cpp
          src/builtin/builtin.cpp
          src/io/io.cpp
          src/joypad/joypad.cpp
          src/dma/dma.cpp
          src/timer/timer.cpp
          src/interrupt/interrupt.cpp)

target_compile_options(core PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>)
target_link_libraries(core PRIVATE range-v3::range-v3 mpark_patterns)
target_include_directories(core PUBLIC ${LR35902_INCLUDE_DIR})
add_library(LR35902::core ALIAS core)

add_library(attaboy debugger/GameBoy.cpp)
target_include_directories(attaboy PUBLIC ${LR35902_INCLUDE_DIR})
target_link_libraries(attaboy PRIVATE LR35902::core)
add_library(LR35902::attaboy ALIAS attaboy)

if(WITH_DEBUGGER)
  find_package(imgui QUIET REQUIRED CONFIG)

  FetchContent_Declare(
    imgui_club
    GIT_REPOSITORY https://github.com/ocornut/imgui_club
    GIT_TAG main
    GIT_SHALLOW 1
    SOURCE_DIR ${LR35902_3RDPARTY_DIR}/imgui_club)
  FetchContent_MakeAvailable(imgui_club)

  add_library(debugView)
  target_sources(debugView PRIVATE src/debugView/debugView.cpp)
  target_include_directories(debugView PUBLIC ${LR35902_INCLUDE_DIR} ${LR35902_3RDPARTY_DIR}/imgui_club)
  add_library(LR35902::debugView ALIAS debugView)

  add_library(imgui_memory_editor INTERFACE)
  target_include_directories(imgui_memory_editor INTERFACE ${LR35902_3RDPARTY_DIR}/imgui_club)
  add_library(imgui::imgui_memory_editor ALIAS imgui_memory_editor)

  target_link_libraries(debugView PRIVATE imgui::imgui imgui::imgui_memory_editor LR35902::core)
  set_target_properties(core debugView PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
endif()

if(SFML_FRONTEND)
  find_package(fmt QUIET REQUIRED CONFIG)
  find_package(CLI11 QUIET REQUIRED)
  find_package(SFML QUIET REQUIRED COMPONENTS graphics CONFIG)

  add_executable(debugger_sfml)
  target_sources(debugger_sfml PRIVATE debugger/sfml/main.cpp)
  target_include_directories(debugger_sfml PUBLIC ${LR35902_INCLUDE_DIR} ${LR35902_SOURCE_DIR})
  target_link_libraries(debugger_sfml PRIVATE LR35902::core LR35902::attaboy fmt::fmt CLI11::CLI11 sfml-graphics)

  if(WITH_DEBUGGER)
    find_package(ImGui-SFML QUIET REQUIRED CONFIG)

    target_link_libraries(debugger_sfml PRIVATE LR35902::debugView imgui::imgui sfml-graphics ImGui-SFML::ImGui-SFML)
    set_target_properties(debugger_sfml PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
  endif()
endif()

if(SDL2_FRONTEND)
  find_package(SDL2 QUIET REQUIRED CONFIG)
  find_package(CLI11 QUIET REQUIRED)

  add_executable(debugger_sdl2)
  target_sources(debugger_sdl2 PRIVATE debugger/sdl2/main.cpp)

  target_include_directories(debugger_sdl2 PUBLIC ${LR35902_INCLUDE_DIR} ${LR35902_SOURCE_DIR})
  target_link_libraries(
    debugger_sdl2 PRIVATE LR35902::core LR35902::attaboy CLI11::CLI11 $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                          $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

  if(WITH_DEBUGGER)
    target_link_libraries(debugger_sdl2 PRIVATE LR35902::debugView imgui::imgui)
    set_target_properties(debugger_sdl2 PROPERTIES COMPILE_DEFINITIONS WITH_DEBUGGER)
  endif()
endif()

if(WITH_TOOLS)
  if(tool_headerdumper)
    find_package(fmt QUIET REQUIRED)
    find_package(CLI11 QUIET REQUIRED)
    find_package(nlohmann_json QUIET REQUIRED CONFIG)

    add_executable(gb.hd tools/headerdumper/main.cpp)
    target_link_libraries(gb.hd PRIVATE fmt::fmt CLI11::CLI11 nlohmann_json::nlohmann_json)
    target_include_directories(gb.hd PRIVATE ${LR35902_INCLUDE_DIR})
    set_target_properties(gb.hd PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LR35902_BINARY_DIR}/tools)
    file(COPY ${LR35902_SOURCE_DIR}/tools/headerdumper/metadata.json DESTINATION ${LR35902_BINARY_DIR}/tools)
  endif()

  if(tool_headerfixer)
    find_package(CLI11 QUIET REQUIRED)

    add_executable(gb.hf tools/headerfixer/main.cpp)
    target_link_libraries(gb.hf PRIVATE LR35902::core CLI11::CLI11 range-v3::range-v3)
    set_target_properties(gb.hf PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LR35902_BINARY_DIR}/tools)
  endif()

  if(tool_disassembler)
    find_package(fmt QUIET REQUIRED)
    find_package(CLI11 QUIET REQUIRED)

    add_executable(gb.dis tools/disassembler/main.cpp)
    target_link_libraries(gb.dis PRIVATE fmt::fmt CLI11::CLI11 range-v3::range-v3)
    set_target_properties(gb.dis PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LR35902_BINARY_DIR}/tools)
  endif()
endif()

if(ROM_TESTS)
  find_program(RGBASM rgbasm REQUIRED)
  find_program(RGBLINK rgblink REQUIRED)
  find_program(RGBFIX rgbfix REQUIRED)

  function(LR35902_add_rom_test)
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
  lr35902_add_rom_test(OUTPUT_DIRECTORY ${LR35902_TEST_DIR}/binary/instructions MBC_TYPE ROM_ONLY SOURCE
                       ${instructions})

  file(GLOB timer CONFIGURE_DEPENDS ${LR35902_TEST_DIR}/source/timer/*.asm)
  lr35902_add_rom_test(OUTPUT_DIRECTORY ${LR35902_TEST_DIR}/binary/timer MBC_TYPE ROM_ONLY SOURCE ${timer})
endif()

if(UNIT_TESTS)
  find_package(Catch2 3 QUIET REQUIRED CONFIG)

  function(LR35902_add_unit_test tgt src)
    add_executable(${tgt})
    target_sources(${tgt} PUBLIC ${src})
    target_sources(${tgt} PUBLIC FILE_SET HEADERS BASE_DIRS ${LR35902_INCLUDE_DIR})
    target_link_libraries(${tgt} PRIVATE LR35902::core Catch2::Catch2WithMain)
    add_test(NAME ${tgt} COMMAND ${tgt})
  endfunction()

  lr35902_add_unit_test(mbc1.test ${LR35902_TEST_DIR}/unit/mbc1.test.cpp)
  lr35902_add_unit_test(mbc2.test ${LR35902_TEST_DIR}/unit/mbc2.test.cpp)
  lr35902_add_unit_test(mbc5.test ${LR35902_TEST_DIR}/unit/mbc5.test.cpp)
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
  set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

  if(analyse_with_cppcheck)
    find_program(cppcheck_command cppcheck REQUIRED)
    set(options --enable=all --inconclusive --language=c++ --std=c++20 --verbose)

    set(CMAKE_CXX_CPPCHECK
        ${cppcheck_command} ${options}
        CACHE STRING "" FORCE)
  endif()

  if(analyse_with_clangtidy)
    find_program(clangtidy_command clang-tidy REQUIRED)
    set(options "--checks=-*,bugprone-*,performance-*,portability-*")

    set(CMAKE_CXX_CLANG_TIDY
        ${clangtidy_command} ${options}
        CACHE STRING "" FORCE)
  endif()

  if(analyse_with_iwyu)
    find_program(iwyu_command include-what-you-use REQUIRED)
    set(options -Xiwyu --cxx17ns --error=100)
  endif()

  if(analyse_with_lwyu)
    set(CMAKE_LINK_WHAT_YOU_USE
        ON
        CACHE BOOL "" FORCE)
  endif()

endif()

set(CPACK_SOURCE_GENERATOR TGZ ZIP)

# cmake-format: off
set(CPACK_SOURCE_IGNORE_FILES 
    \.git/ 
    \.cache/ 
    .*\.swp$ 
    ${LR35902_BINARY_DIR}
)
# cmake-format: on

set(CPACK_GENERATOR TGZ ZIP)

set(targets_to_package)
if(TARGET debugger_sdl2)
  list(APPEND targets_to_package debugger_sdl2)
endif()

if(TARGET debugger_sfml)
  list(APPEND targets_to_package debugger_sfml)
endif()

if(WITH_TOOLS)
  if(TARGET gb.hd)
    list(APPEND targets_to_package gb.hd)
  endif()

  if(TARGET gb.hf)
    list(APPEND targets_to_package gb.hf)
  endif()

  if(TARGET gb.dis)
    list(APPEND targets_to_package gb.dis)
  endif()
endif()

if(targets_to_package)
  install(TARGETS ${targets_to_package})
endif()

include(CPack)
