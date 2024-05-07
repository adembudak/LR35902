get-dependencies:
	vcpkg install sdl2 sfml cli11 fmt range-v3 imgui imgui-sfml imgui[sdl2-binding] imgui[sdl2-renderer-binding]

configure:
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B build

reconfigure: 
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B build --fresh

build:
	cmake --build build

rebuild:
	cmake --build build --clean-first

fresh-build:
	just reconfigure
	just build

lint:
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -DANALYSE_WITH_CPPCHECK:BOOL=ON -DANALYSE_WITH_CLANGTIDY:BOOL=ON -DANALYSE_WITH_IWYU:BOOL:ON -DANALYSE_WITH_LWYU:BOOL=ON -S . -B build_static_analysis --fresh
	cmake --build build_static_analysis

tui:
	ccmake -S . -B build
