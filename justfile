set windows-shell := ["powershell", "-NoLogo"]

default:
	@just --list --unsorted

get-dependencies:
	vcpkg integrate install
	vcpkg install sdl2 sfml cli11 fmt range-v3 imgui imgui-sfml imgui[sdl2-binding] imgui[sdl2-renderer-binding] nlohmann-json catch2

builddir := 'build'
configure dir = builddir:
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B {{dir}}

reconfigure dir = builddir:
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B {{dir}} --fresh

tgt := 'all'
build dir = builddir target = tgt:
	cmake --build {{dir}} --target {{target}}

rebuild dir = builddir:
	cmake --build {{dir}} --clean-first

fresh-build dir = builddir:
	just reconfigure {{dir}}
	just build {{dir}}

test dir = builddir:
	ctest --test-dir {{dir}}

package-sources dir = builddir:
	cmake --build {{dir}} --target package_source

package-binaries dir = builddir:
	cmake --build {{dir}} --target package

package dir = builddir:
	just package-sources {{dir}}
	just package-binaries {{dir}}

lint:
	cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -DCMAKE_BUILD_TYPE:STRING="Debug" -DANALYSE_WITH_CPPCHECK:BOOL=1 -DSTATIC_ANALYSIS:BOOL=1 -DANALYSE_WITH_CLANGTIDY:BOOL=1 -DANALYSE_WITH_IWYU:BOOL=1 -DANALYSE_WITH_LWYU:BOOL=1 -S . -B build_static_analysis --fresh
	cmake --build build_static_analysis

[confirm('Are you sure you want to remove this directory? [N|y]')]
remove dir:
	cmake -E rm -rf {{dir}}

[linux, unix]
tui dir = builddir:
	ccmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B {{dir}}

gui dir = builddir:
	cmake-gui -S . -B {{dir}}

[linux, unix]
meson builddir:
	meson setup -Dwith_debugger=true \
	-Dsdl2_frontend=false \
	-Dsfml_frontend=false \
	-Dwith_tools=true \
	--wrap-mode default \
	--default-library both {{builddir}}
	meson compile -C {{builddir}}

[windows]
meson builddir:
	meson setup --backend vs2022 \
	 -Dwith_debugger=false \
	 -Dsdl2_frontend=false \
	 -Dsfml_frontend=false \
	 -Dwith_tools=true \
	 --wrap-mode forcefallback {{builddir}}
	meson compile -C {{builddir}}
