project('LR35902', 'cpp',  default_options : ['cpp_std=c++2a'])

LR35902_incdir = include_directories('include') 

ranges_dep = dependency('range-v3', required: true)

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
         'src/timer/timer.cpp']

lr35902_core = library('lr35902', 
  sources: lr35902_sources, 
  dependencies : ranges_dep,
  include_directories : LR35902_incdir)

if get_option('with_debugger')
  if get_option('sdl2_frontend')
    executable('debugger_sdl', 
      sources:['debugger/sdl/main.cpp', 'debugger/GameBoy.cpp'],
      dependencies : [ranges_dep, lr35902_core],
      include_directories : LR35902_incdir)
  endif
endif

