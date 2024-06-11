project(
    'LR35902',
    'cpp',
    default_options: ['cpp_std=c++20', 'wrap_mode=forcefallback'],
    license: 'BSL-1.0',
    license_files: 'license',
    meson_version: '1.4.0',
    subproject_dir: 'subprojects',
)

LR35902_incdir = include_directories('include')

ranges_dep = dependency('range-v3', default_options: {'tests': false}, required: true)

sdl2_dep = dependency('sdl2', default_options: {'test': false, 'assertions': 'disabled', 'use_render': 'enabled'}, required: true,)

imgui_dep = dependency(
    'imgui',
    default_options: {
        'sdl2': 'enabled',
        'sdl_renderer': 'enabled',
        'opengl': 'enabled',
        'dx9': 'disabled',
        'dx10': 'disabled',
        'dx11': 'disabled',
        'dx12': 'disabled',
        'metal': 'disabled',
        'vulkan': 'disabled',
        'webgpu': 'disabled',
        'glfw': 'disabled',
        'osx': 'disabled',
        'win': 'disabled',
        'allegro5': 'disabled',
    },
    required: true,
)

cli11_dep = dependency('cli11', default_options: {'tests': false}, required: true)
fmt_dep = dependency('fmt', required: true)

#sfml_dep = dependency('sfml', required: true)
#imgui_sfml_dep = dependency('imgui-sfml', required: true)

lr35902_sources = [
    'src/bootrom/bootrom.cpp',
    'src/builtin/builtin.cpp',
    'src/bus/bus.cpp',
    'src/cartridge/cartridge.cpp',
    'src/cartridge/header/header.cpp',
    'src/cartridge/kind/mbc1/mbc1_128kb.cpp',
    'src/cartridge/kind/mbc1/mbc1_1mb.cpp',
    'src/cartridge/kind/mbc1/mbc1_256kb.cpp',
    'src/cartridge/kind/mbc1/mbc1_2mb.cpp',
    'src/cartridge/kind/mbc1/mbc1_32kb.cpp',
    'src/cartridge/kind/mbc1/mbc1_512kb.cpp',
    'src/cartridge/kind/mbc1/mbc1_64kb.cpp',
    'src/cartridge/kind/mbc1_ram/mbc1_32kb_32kb.cpp',
    'src/cartridge/kind/mbc1_ram/mbc1_ram.cpp',
    'src/cartridge/kind/mbc2/mbc2.cpp',
    'src/cartridge/kind/mbc5/mbc5.cpp',
    'src/cartridge/kind/rom_only/rom_only.cpp',
    'src/cartridge/kind/rom_ram/rom_ram.cpp',
    'src/cpu/cpu.cpp',
    'src/cpu/registers/r16.cpp',
    'src/cpu/registers/r8.cpp',
    'src/dma/dma.cpp',
    'src/interrupt/interrupt.cpp',
    'src/io/io.cpp',
    'src/joypad/joypad.cpp',
    'src/ppu/ppu.cpp',
    'src/timer/timer.cpp',
]

lr35902_core = library(
    'lr35902',
    sources: lr35902_sources,
    dependencies: [ranges_dep, imgui_dep],
    include_directories: LR35902_incdir,
)

if get_option('with_debugger')
    debugView = library(
        'debugView',
        cpp_args: '-DWITH_DEBUGGER',
        sources: 'src/debugView/debugView.cpp',
        link_with: lr35902_core,
        dependencies: imgui_dep,
        include_directories: [LR35902_incdir, '3rdparty/imgui_club'],
    )

    if get_option('sdl2_frontend')
        executable(
            'debugger_sdl',
            sources: ['debugger/sdl2/main.cpp', 'debugger/GameBoy.cpp'],
            cpp_args: '-DWITH_DEBUGGER',
            link_with: [lr35902_core, debugView],
            dependencies: [ranges_dep, sdl2_dep, imgui_dep, cli11_dep],
            include_directories: LR35902_incdir,
        )
    endif

    # if get_option('sfml_frontend')
    #    executable('debugger_sfml',
    #      sources: ['debugger/sfml/main.cpp', 'debugger/GameBoy.cpp'],
    #      cpp_args : '-DWITH_DEBUGGER',
    #      link_args: '-ldl',
    #      link_with: [lr35902_core, debugView],
    #      dependencies : [ranges_dep, imgui_dep, sfml_dep, imgui_sfml_dep, cli11_dep, fmt_dep],
    #      include_directories : LR35902_incdir)
    #  endif

endif

if get_option('with_tools')
    executable(
        'gb.hd',
        'tools/header_dumper.cpp',
        include_directories: LR35902_incdir,
        link_with: lr35902_core,
        dependencies: [fmt_dep, cli11_dep, ranges_dep],
    )

    executable(
        'gb.hf',
        'tools/header_fixer.cpp',
        include_directories: LR35902_incdir,
        link_with: lr35902_core,
        dependencies: [cli11_dep, ranges_dep],
    )

    executable(
        'gb.dis',
        'tools/disassembler.cpp',
        dependencies: [fmt_dep, cli11_dep, ranges_dep],
    )
endif
