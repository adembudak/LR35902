set windows-shell := ["powershell", "-NoLogo"]

default:
	@just --list --unsorted

get-dependencies:
	vcpkg install sdl2 sfml cli11 fmt range-v3 imgui imgui-sfml imgui[sdl2-binding] imgui[sdl2-renderer-binding]
	vcpkg integrate install

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
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -DCMAKE_BUILD_TYPE:STRING="Debug" -DANALYSE_WITH_CPPCHECK:BOOL=1 -DSTATIC_ANALYSIS:BOOL=1 -DANALYSE_WITH_CLANGTIDY:BOOL=1 -DANALYSE_WITH_IWYU:BOOL=1 -DANALYSE_WITH_LWYU:BOOL=1 -S . -B build_static_analysis --fresh
	cmake --build build_static_analysis

[linux]
tui:
	ccmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B build

gui:
	cmake-gui -S . -B build

[linux]
meson builddir:
	meson setup -Dwith_debugger=true \
	-Dsdl2_frontend=true \
	-Dsfml_frontend=false \
	-Dwith_tools=true \
	--wrap-mode forcefallback \
	--default-library both {{builddir}}
	meson compile -C {{builddir}}

[windows]
meson builddir:
	meson setup --backend vs2022 \
	 -Dwith_debugger=false \
	 -Dsdl2_frontend=false \
	 -Dsfml_frontend=false \
	 -Dwith_tools=false \
	 --wrap-mode forcefallback {{builddir}}
	meson compile -C {{builddir}}
