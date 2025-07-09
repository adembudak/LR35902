set windows-shell := ["powershell", "-NoLogo"]

default:
  @just --list --unsorted

get-dependencies:
  vcpkg install
  vcpkg integrate install

builddir := 'build'
configure dir = builddir:
  cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B {{dir}}

reconfigure dir = builddir:
  cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -DCMAKE_CXX_STANDARD:STRING=20 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=1 -S . -B {{dir}} --fresh

tgt := if os_family() == "windows" { 'ALL_BUILD' } else { 'all' }

alias b := build
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

[windows]
hack dir = builddir:
  cmake --open {{dir}}

[linux, unix]
meson builddir:
  meson setup -Dwith_debugger=true \
  -D with_tools=false \
  -D unit_tests=true \
  --wrap-mode nofallback --force-fallback-for=mpark-patterns,cli11,catch2,imgui \
  --default-library both . {{builddir}}
  meson compile -C {{builddir}}

[windows]
meson builddir:
  meson setup --backend vs2022 \
  -D with_debugger=true \
  -D with_tools=true \
  -D unit_tests=true \
  --wrap-mode forcefallback . {{builddir}}
  meson compile -C {{builddir}}


run:
  just build
  ./build/Debug/debugger
