get-dependencies:
	vcpkg install sdl2 sfml cli11 fmt range-v3 imgui imgui-sfml imgui[sdl2-binding] imgui[sdl2-renderer-binding]

configure:
	cmake -S . -B build

build:
	cmake --build build

fresh-build:
	cmake -S . -B build --fresh
	just build

tui:
	ccmake -S . -B build
