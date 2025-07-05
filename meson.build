project(
  'LR35902',
  'cpp',
  default_options: ['cpp_std=c++20', 'wrap_mode=forcefallback'],
  license: 'BSL-1.0',
  license_files: 'license',
  subproject_dir: 'subprojects',
  version: '0.1.0',
  meson_version: '>=1.6.1',
)

if meson.get_compiler('cpp').get_id() == 'msvc'
  add_project_arguments('/Zc:__cplusplus', language: 'cpp')
endif

LR35902_incdir = include_directories('include')
LR35902_sourcedir = include_directories('.')

ranges_dep = dependency('range-v3', required: true)
patterns_dep = dependency('mpark_patterns', required: true)
cli11_dep = dependency('cli11', default_options: {'tests': false, 'single-file-header': false}, required: false)

lr35902_sources = files(
  'src/builtin/builtin.cpp',
  'src/bus/bus.cpp',
  'src/cartridge/cartridge.cpp',
  'src/cartridge/header/header.cpp',
  'src/cartridge/kind/mbc1.cpp',
  'src/cartridge/kind/mbc2.cpp',
  'src/cartridge/kind/mbc3.cpp',
  'src/cartridge/kind/mbc5.cpp',
  'src/cartridge/kind/rom_only.cpp',
  'src/cartridge/kind/rom_ram.cpp',
  'src/cpu/cpu.cpp',
  'src/cpu/registers/r16.cpp',
  'src/cpu/registers/r8.cpp',
  'src/dma/dma.cpp',
  'src/interrupt/interrupt.cpp',
  'src/io/io.cpp',
  'src/joypad/joypad.cpp',
  'src/ppu/ppu.cpp',
  'src/timer/timer.cpp',
)

lr35902_core = library(
  'lr35902',
  sources: lr35902_sources,
  dependencies: [ranges_dep, patterns_dep],
  include_directories: LR35902_incdir,
)

attaboy = library(
  'attaboy',
  sources: 'backend/Emu.cpp',
  link_with: lr35902_core,
  include_directories: [LR35902_sourcedir, LR35902_incdir],
)

if get_option('with_debugger')
  imgui_dep = dependency('imgui', default_options: {'opengl': 'enabled'}, required: false)

  debugView = library(
    'debugView',
    cpp_args: '-DWITH_DEBUGGER',
    sources: 'src/debugView/debugView.cpp',
    link_with: lr35902_core,
    dependencies: imgui_dep,
    include_directories: [LR35902_incdir, '3rdparty/imgui_club'],
  )

  executable(
    'debugger',
    sources: ['debugger/main.cpp'],
    cpp_args: '-DWITH_DEBUGGER',
    link_with: [lr35902_core, debugView, attaboy],
    dependencies: [ranges_dep, imgui_dep, cli11_dep],
    include_directories: LR35902_incdir,
  )
endif

if get_option('with_tools')
  nlohmann_json_dep = dependency('nlohmann_json', required: true)
  fmt_dep = dependency('fmt', required: true)

  executable(
    'gb.hd',
    'tools/headerdumper/main.cpp',
    include_directories: LR35902_incdir,
    dependencies: [fmt_dep, cli11_dep, nlohmann_json_dep],
  )

  executable(
    'gb.hf',
    'tools/headerfixer/main.cpp',
    include_directories: LR35902_incdir,
    link_with: lr35902_core,
    dependencies: [cli11_dep, ranges_dep],
  )

  executable('gb.dis', 'tools/disassembler/main.cpp', dependencies: [fmt_dep, cli11_dep, ranges_dep])
endif


if (get_option('unit_tests'))
  catch2_dep = dependency('catch2-with-main', default_options: {'tests': false}, version: '>=3.8.0', required: true)

  foreach f : ['mbc1.test', 'mbc2.test', 'mbc3.test', 'mbc5.test']
    test_executable = executable(
      f,
      'tests/unit/' + f + '.cpp',
      include_directories: LR35902_incdir,
      link_with: lr35902_core,
      dependencies: catch2_dep,
    )
    test(f, test_executable)
  endforeach
endif
