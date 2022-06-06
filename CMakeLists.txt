cmake_minimum_required(VERSION 3.23)
project(LR3590 LANGUAGES CXX)

set(LR3590_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(WITH_DEBUGGER "" OFF)

add_library(core src/core.cpp 
        src/registers/r16.cpp
        src/registers/r8.cpp
        )

add_library(bus src/stubs/bus/bus.cpp)
target_include_directories(bus PRIVATE include)

target_link_libraries(core PRIVATE bus)
target_include_directories(core PUBLIC ${LR3590_INCLUDE_DIR})

if (WITH_DEBUGGER)
  add_subdirectory(3rdparty)

  add_library(debugView src/debugView/debugView.cpp)
  target_link_libraries(debugView PUBLIC imgui core)
  target_include_directories(debugView PUBLIC ${LR3590_INCLUDE_DIR} 3rdparty/imgui-1.87)

  add_executable(test tests/main.cpp)
  target_link_libraries(test PUBLIC debugView)
  target_include_directories(test PUBLIC ${LR3590_INCLUDE_DIR})
endif()

