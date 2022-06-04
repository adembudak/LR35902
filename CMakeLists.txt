cmake_minimum_required(VERSION 3.23)
project(LR3590 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)

add_library(core src/core.cpp 
        src/immediate/e8.cpp
        src/immediate/n8.cpp
        src/immediate/n16.cpp
        src/registers/r16.cpp
        src/registers/r8.cpp
        )

add_library(bus src/stubs/bus/bus.cpp)
target_include_directories(bus PRIVATE include)
target_link_libraries(core PRIVATE bus)

target_include_directories(core PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty/imgui-1.87)
target_link_libraries(core PUBLIC imgui)
target_compile_options(core PRIVATE 
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover=all
        -fsanitize=float-divide-by-zero
        -fsanitize=float-cast-overflow
        -fno-sanitize=null
        -fno-sanitize=alignment
        -Waddress
        -Wuninitialized
        -Wreorder
        -Wall
        -Wextra
        -Wconversion
        -Warith-conversion)

if (WITH_DEBUGGER)
  add_subdirectory(3rdparty)
  add_executable(test tests/main.cpp)
  target_include_directories(test PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty/imgui-1.87)
  target_link_libraries(test PUBLIC core)
endif()

