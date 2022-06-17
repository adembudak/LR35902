find_package(SDL2 REQUIRED)

add_library(
  imgui
  imgui-1.87/imgui.cpp imgui-1.87/imgui_demo.cpp imgui-1.87/imgui_draw.cpp
  imgui-1.87/imgui_tables.cpp imgui-1.87/imgui_widgets.cpp)

target_include_directories(imgui PUBLIC imgui-1.87)

add_library(
  imgui_impl_sdlrenderer imgui-1.87/backends/imgui_impl_sdl.cpp
                         imgui-1.87/backends/imgui_impl_sdlrenderer.cpp)

target_include_directories(imgui_impl_sdlrenderer
                           PUBLIC imgui-1.87 imgui-1.87/backends imgui_club)

target_link_libraries(imgui_impl_sdlrenderer PRIVATE SDL2::SDL2
                                                     SDL2::SDL2-static)
target_link_libraries(imgui PUBLIC imgui_impl_sdlrenderer)
