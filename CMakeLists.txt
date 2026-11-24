cmake_minimum_required(VERSION 3.10)
project(NESEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(nes_emu 
    src/main.cpp
    src/Bus.cpp
    src/CPU6502.cpp
    src/Mapper.cpp
    src/Mapper_000.cpp
    src/Cartridge.cpp
    src/PPU2C02.cpp
)

target_include_directories(nes_emu PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(nes_emu PRIVATE ${SDL2_LIBRARIES})
