add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

set_languages("c++20")
add_requires("vcpkg::range-v3", "vcpkg::fmt", "vcpkg::cli11")

target("core")
  set_kind("static")
  add_files("src/cpu/cpu.cpp",
          "src/cpu/registers/r16.cpp",
          "src/cpu/registers/r8.cpp",
          "src/bus/bus.cpp",
          "src/cartridge/cartridge.cpp",
          "src/cartridge/header/header.cpp",
          "src/cartridge/kind/rom_only/rom_only.cpp",
          "src/cartridge/kind/rom_ram/rom_ram.cpp",
          "src/cartridge/kind/mbc1/mbc1_32kb.cpp",
          "src/cartridge/kind/mbc1/mbc1_64kb.cpp",
          "src/cartridge/kind/mbc1/mbc1_128kb.cpp",
          "src/cartridge/kind/mbc1/mbc1_256kb.cpp",
          "src/cartridge/kind/mbc1/mbc1_512kb.cpp",
          "src/cartridge/kind/mbc1/mbc1_1mb.cpp",
          "src/cartridge/kind/mbc1/mbc1_2mb.cpp",
          "src/cartridge/kind/mbc1_ram/mbc1_ram.cpp",
          "src/cartridge/kind/mbc1_ram/mbc1_32kb_32kb.cpp",
          "src/cartridge/kind/mbc2/mbc2.cpp",
          "src/cartridge/kind/mbc5/mbc5.cpp",
          "src/bootrom/bootrom.cpp",
          "src/ppu/ppu.cpp",
          "src/builtin/builtin.cpp",
          "src/io/io.cpp",
          "src/joypad/joypad.cpp",
          "src/dma/dma.cpp",
          "src/timer/timer.cpp",
          "src/interrupt/interrupt.cpp")
  add_includedirs("include")
  add_packages("vcpkg::range-v3")
target_end()

option("with debugger")
    set_default(false)
    set_showmenu(true)
    --
    -- getting there 
    --
option_end()

option("with tools")
    set_default(false)
    set_showmenu(true)

    target("gb.hd")
      set_kind("binary")
      add_files("tools/header_dumper.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("vcpkg::range-v3", "vcpkg::fmt", "vcpkg::CLI11")
    target_end()

    target("gb.hf")
      set_kind("binary")
      add_files("tools/header_fixer.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("vcpkg::range-v3", "vcpkg::CLI11")
    target_end()

    target("gb.dis")
      set_kind("binary")
      add_files("tools/disassembler.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("vcpkg::range-v3", "vcpkg::fmt", "vcpkg::CLI11")
    target_end()
option_end()
