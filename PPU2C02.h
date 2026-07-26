#pragma once
#include <cstdint>
#include <memory>
#include <array>

class Cartridge;

class PPU2C02 {
public:
    PPU2C02();
    ~PPU2C02();

    // Connect Cartridge to PPU Bus
    void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);

    // CPU Bus read/write
    uint8_t cpuRead(uint16_t addr, bool rdonly = false);
    void cpuWrite(uint16_t addr, uint8_t data);

    // PPU Bus read/write
    uint8_t ppuRead(uint16_t addr, bool rdonly = false);
    void ppuWrite(uint16_t addr, uint8_t data);

    void clock();

    // Debugging utilities
    std::array<uint32_t, 128 * 128> GetPatternTable(uint8_t i, uint8_t palette);

    // The screen buffer (256x240 pixels)
    // Using ARGB format for SDL2 compatibility
    std::array<uint32_t, 256 * 240> videoBuffer;

    bool nmi = false; // Non-Maskable Interrupt signal

    int16_t scanline = 0;
    int16_t cycle = 0;
    struct sObjectAttributeEntry {
        uint8_t y;
        uint8_t id;
        uint8_t attribute;
        uint8_t x;
    };

    sObjectAttributeEntry OAM[64];
    uint8_t* pOAM = (uint8_t*)OAM;
    uint8_t oam_addr = 0x00;

private:
    std::shared_ptr<Cartridge> cart;

    // VRAM Name Tables
    std::array<std::array<uint8_t, 1024>, 2> tblName;
    // Palette RAM
    std::array<uint8_t, 32> tblPalette;

    // PPU Registers (Simplified)
    uint8_t status = 0x00;
    uint8_t control = 0x00;
    uint8_t mask = 0x00;

    // Internal address registers
    uint16_t vram_addr = 0x0000;
    uint16_t tram_addr = 0x0000;
    uint8_t fine_x = 0x00;
    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;

    uint32_t GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);

    std::array<uint32_t, 64> palScreen;
};
