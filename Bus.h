#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "PPU2C02.h"

class CPU6502; // Forward declaration
class Cartridge;

class Bus {
public:
    Bus();
    ~Bus();

    // Devices on the bus
    std::shared_ptr<Cartridge> cart;
    PPU2C02 ppu;
    uint32_t nSystemClockCounter = 0;

    // Read and Write to the bus
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr, bool bReadOnly = false);

    void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);

    // Controllers
    uint8_t controller[2];

private:
    // 2KB RAM for NES
    std::array<uint8_t, 2048> ram;
    // Controller state
    uint8_t controller_state[2];
};
