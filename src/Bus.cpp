#include "Bus.h"
#include "Cartridge.h"

Bus::Bus() {
    // Clear RAM
    ram.fill(0x00);
}

Bus::~Bus() {
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    cart = cartridge;
    ppu.ConnectCartridge(cartridge);
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (cart && cart->cpuWrite(addr, data)) {
        // Cartridge handled the write
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        ram[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        ppu.cpuWrite(addr & 0x0007, data);
    } else if (addr >= 0x4000 && addr <= 0x4013) {
        // APU Registers
    } else if (addr == 0x4014) {
        // OAM DMA
        uint16_t dma_addr = data << 8;
        for (int i = 0; i < 256; i++) {
            ppu.pOAM[ppu.oam_addr] = read(dma_addr + i);
            ppu.oam_addr++;
        }
    } else if (addr >= 0x4016 && addr <= 0x4017) {
        controller_state[addr & 0x0001] = controller[addr & 0x0001];
    }
}

uint8_t Bus::read(uint16_t addr, bool bReadOnly) {
    uint8_t data = 0x00;
    if (cart && cart->cpuRead(addr, data)) {
        // Cartridge handled the read
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = ram[addr & 0x07FF];
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        data = ppu.cpuRead(addr & 0x0007, bReadOnly);
    } else if (addr >= 0x4016 && addr <= 0x4017) {
        data = (controller_state[addr & 0x0001] & 0x80) > 0;
        controller_state[addr & 0x0001] <<= 1;
    }
    return data;
}
