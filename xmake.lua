set_languages("c++20")

add_requires("range-v3")

add_requires("fmt")
add_requires("cli11")

add_requires("libsdl", {configs = {wayland = false}})
add_requires("sfml")

add_requires("imgui", {configs = {sdl2_renderer = true,sdl2 = true}})
add_requires("imgui-sfml")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

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
  add_packages("range-v3")

  if has_config("with_debugger") then 
      add_defines("WITH_DEBUGGER")
  end
target_end()

target("thing")
   set_kind("static")
   add_files("debugger/GameBoy.cpp")
   add_deps("core")
   add_includedirs("include")
target_end()

option("with_debugger", {default = false, showmenu = true})
    target("debugView")
       set_kind("static")
       add_files("src/debugView/debugView.cpp")
       add_deps("core")
       add_packages("imgui")
       add_includedirs("include", "imgui_club")
       add_defines("WITH_DEBUGGER")
    target_end()
option_end()

option("sdl2_frontend", {default = false, showmenu = true})
   target("debugger_sdl")
     set_kind("binary")
     add_files("debugger/sdl2/main.cpp")
     add_includedirs("include") 
     add_deps("core", "thing")
     add_packages("fmt", "cli11", "libsdl")

     if has_config("with_debugger") then 
       add_defines("WITH_DEBUGGER")
       add_deps("debugView")
       add_packages("imgui")
     end
   target_end()
option_end()

option("sfml_frontend", {default = false, showmenu = true})
   target("debugger_sfml")
     set_kind("binary")
     add_files("debugger/sfml/main.cpp")
     add_includedirs("include") 
     add_deps("core", "thing")
     add_packages("fmt", "cli11", "sfml")

     if has_config("with_debugger") then 
       add_defines("WITH_DEBUGGER")
       add_deps("debugView")
       add_packages("imgui", "imgui-sfml")
     end
    target_end()
option_end()

option("with_tools", {default = false, showmenu = true})
    target("gb.hd")
      set_kind("binary")
      add_files("tools/header_dumper.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("range-v3", "fmt", "cli11")
    target_end()

    target("gb.hf")
      set_kind("binary")
      add_files("tools/header_fixer.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("range-v3", "cli11")
    target_end()

    target("gb.dis")
      set_kind("binary")
      add_files("tools/disassembler.cpp")
      add_includedirs("include")
      add_deps("core")
      add_packages("range-v3", "fmt", "cli11")
    target_end()
option_end()
