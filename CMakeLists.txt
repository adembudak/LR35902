cmake_minimum_required(VERSION 3.23)
project(LR3590 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)

if (WITH_DEBUGGER)
  add_subdirectory(3rdparty)
endif()

add_library(core src/core.cpp)
target_include_directories(core PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty/imgui-1.87)
target_link_libraries(core PUBLIC imgui)

add_executable(test tests/main.cpp)
target_include_directories(test PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty/imgui-1.87)
target_link_libraries(test PUBLIC core)

