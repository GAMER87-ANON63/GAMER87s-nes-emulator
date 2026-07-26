#include "PPU2C02.h"
#include "Cartridge.h"
#include <cstdlib>

PPU2C02::PPU2C02() {
    videoBuffer.fill(0xFF000000); // Fill with black

    palScreen[0x00] = 0xFF545454; palScreen[0x01] = 0xFF001E74; palScreen[0x02] = 0xFF081090; palScreen[0x03] = 0xFF300088;
    palScreen[0x04] = 0xFF440064; palScreen[0x05] = 0xFF5C0030; palScreen[0x06] = 0xFF540400; palScreen[0x07] = 0xFF3C1800;
    palScreen[0x08] = 0xFF202A00; palScreen[0x09] = 0xFF083A00; palScreen[0x0A] = 0xFF004000; palScreen[0x0B] = 0xFF003C00;
    palScreen[0x0C] = 0xFF00323C; palScreen[0x0D] = 0xFF000000; palScreen[0x0E] = 0xFF000000; palScreen[0x0F] = 0xFF000000;

    palScreen[0x10] = 0xFF989698; palScreen[0x11] = 0xFF084CC4; palScreen[0x12] = 0xFF3032EC; palScreen[0x13] = 0xFF5C1EE4;
    palScreen[0x14] = 0xFF8814B0; palScreen[0x15] = 0xFFA01464; palScreen[0x16] = 0xFF982220; palScreen[0x17] = 0xFF783C00;
    palScreen[0x18] = 0xFF545A00; palScreen[0x19] = 0xFF287200; palScreen[0x1A] = 0xFF087C00; palScreen[0x1B] = 0xFF007628;
    palScreen[0x1C] = 0xFF006678; palScreen[0x1D] = 0xFF000000; palScreen[0x1E] = 0xFF000000; palScreen[0x1F] = 0xFF000000;

    palScreen[0x20] = 0xFFECEEEC; palScreen[0x21] = 0xFF4C9AEC; palScreen[0x22] = 0xFF787CEC; palScreen[0x23] = 0xFFB062EC;
    palScreen[0x24] = 0xFFE454EC; palScreen[0x25] = 0xFFEC58B4; palScreen[0x26] = 0xFFEC6A64; palScreen[0x27] = 0xFFD48820;
    palScreen[0x28] = 0xFFA0AA00; palScreen[0x29] = 0xFF74C400; palScreen[0x2A] = 0xFF4CD020; palScreen[0x2B] = 0xFF38CC6C;
    palScreen[0x2C] = 0xFF38B4CC; palScreen[0x2D] = 0xFF3C3C3C; palScreen[0x2E] = 0xFF000000; palScreen[0x2F] = 0xFF000000;

    palScreen[0x30] = 0xFFECEEEC; palScreen[0x31] = 0xFFA8CCEC; palScreen[0x32] = 0xFFBCBCEC; palScreen[0x33] = 0xFFD4B2EC;
    palScreen[0x34] = 0xFFECAEEC; palScreen[0x35] = 0xFFECAED4; palScreen[0x36] = 0xFFECB4B0; palScreen[0x37] = 0xFFE4C490;
    palScreen[0x38] = 0xFFCCD278; palScreen[0x39] = 0xFFB4DE78; palScreen[0x3A] = 0xFFA8E290; palScreen[0x3B] = 0xFF98E2B4;
    palScreen[0x3C] = 0xFFA0D6E4; palScreen[0x3D] = 0xFFA0A2A0; palScreen[0x3E] = 0xFF000000; palScreen[0x3F] = 0xFF000000;
}

PPU2C02::~PPU2C02() {
}

void PPU2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    cart = cartridge;
}

uint32_t PPU2C02::GetColorFromPaletteRam(uint8_t palette, uint8_t pixel) {
    return palScreen[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

uint8_t PPU2C02::cpuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
    switch (addr) {
        case 0x0000: break; // Control
        case 0x0001: break; // Mask
        case 0x0002: 
            data = (status & 0xE0) | (ppu_data_buffer & 0x1F);
            status &= ~0x80; // Clear VBlank flag on read
            address_latch = 0;
            break; // Status
        case 0x0003: break; // OAM Address
        case 0x0004: data = pOAM[oam_addr]; break; // OAM Data
        case 0x0005: break; // Scroll
        case 0x0006: break; // PPU Address
        case 0x0007: // PPU Data
            data = ppu_data_buffer;
            ppu_data_buffer = ppuRead(vram_addr);
            if (vram_addr >= 0x3F00) data = ppu_data_buffer;
            vram_addr += (control & 0x04) ? 32 : 1;
            break; 
    }
    return data;
}

void PPU2C02::cpuWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x0000: // Control
            control = data;
            tram_addr = (tram_addr & 0xF3FF) | ((data & 0x03) << 10);
            break; 
        case 0x0001: mask = data; break; // Mask
        case 0x0002: break; // Status is read-only
        case 0x0003: oam_addr = data; break; // OAM Address
        case 0x0004: pOAM[oam_addr] = data; break; // OAM Data
        case 0x0005: // Scroll
            if (address_latch == 0) {
                fine_x = data & 0x07;
                tram_addr = (tram_addr & 0xFFE0) | (data >> 3);
                address_latch = 1;
            } else {
                tram_addr = (tram_addr & 0x8FFF) | ((data & 0x07) << 12);
                tram_addr = (tram_addr & 0xFC1F) | ((data & 0xF8) << 2);
                address_latch = 0;
            }
            break; 
        case 0x0006: // PPU Address
            if (address_latch == 0) {
                tram_addr = (tram_addr & 0x00FF) | ((data & 0x3F) << 8);
                address_latch = 1;
            } else {
                tram_addr = (tram_addr & 0xFF00) | data;
                vram_addr = tram_addr;
                address_latch = 0;
            }
            break; 
        case 0x0007: // PPU Data
            ppuWrite(vram_addr, data);
            vram_addr += (control & 0x04) ? 32 : 1;
            break; 
    }
}

uint8_t PPU2C02::ppuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;

    if (cart && cart->ppuRead(addr, data)) {
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = tblName[(addr & 0x1000) >> 12][addr & 0x0FFF];
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        // Vertical Mirroring
        if (addr >= 0x0000 && addr <= 0x03FF) data = tblName[0][addr & 0x03FF];
        if (addr >= 0x0400 && addr <= 0x07FF) data = tblName[1][addr & 0x03FF];
        if (addr >= 0x0800 && addr <= 0x0BFF) data = tblName[0][addr & 0x03FF];
        if (addr >= 0x0C00 && addr <= 0x0FFF) data = tblName[1][addr & 0x03FF];
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        data = tblPalette[addr] & (mask & 0x01 ? 0x30 : 0x3F);
    }
    return data;
}

void PPU2C02::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;

    if (cart && cart->ppuWrite(addr, data)) {
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        tblName[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        // Vertical Mirroring
        if (addr >= 0x0000 && addr <= 0x03FF) tblName[0][addr & 0x03FF] = data;
        if (addr >= 0x0400 && addr <= 0x07FF) tblName[1][addr & 0x03FF] = data;
        if (addr >= 0x0800 && addr <= 0x0BFF) tblName[0][addr & 0x03FF] = data;
        if (addr >= 0x0C00 && addr <= 0x0FFF) tblName[1][addr & 0x03FF] = data;
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        tblPalette[addr] = data;
    }
}

void PPU2C02::clock() {
    if (scanline >= 0 && scanline < 240 && cycle == 256) {
        // Render Background Scanline
        if (mask & 0x08) {
            uint16_t bg_pattern_table = (control & 0x10) ? 0x1000 : 0x0000;
            int scroll_x = fine_x; // Simplified
            
            for (int x = 0; x < 256; x++) {
                uint8_t tile_x = (x + scroll_x) / 8;
                uint8_t tile_y = scanline / 8;
                
                uint16_t nametable_addr = 0x2000 | (vram_addr & 0x0FFF);
                uint8_t tile_id = ppuRead(nametable_addr + tile_y * 32 + tile_x, false);
                
                // Fetch Attribute
                uint16_t attribute_addr = 0x23C0 | (vram_addr & 0x0C00) | ((tile_y / 4) * 8) + (tile_x / 4);
                uint8_t attr_byte = ppuRead(attribute_addr, false);
                uint8_t attr_shift = ((tile_y % 4) / 2) * 4 + ((tile_x % 4) / 2) * 2;
                uint8_t palette = (attr_byte >> attr_shift) & 0x03;
                
                uint16_t pattern_addr = bg_pattern_table + (tile_id * 16) + (scanline % 8);
                uint8_t tile_lsb = ppuRead(pattern_addr, false);
                uint8_t tile_msb = ppuRead(pattern_addr + 8, false);
                
                uint8_t pixel_col = 7 - ((x + scroll_x) % 8);
                uint8_t pixel = ((tile_lsb >> pixel_col) & 0x01) | (((tile_msb >> pixel_col) & 0x01) << 1);
                
                if (pixel != 0) {
                    videoBuffer[scanline * 256 + x] = palScreen[ppuRead(0x3F00 + (palette << 2) + pixel, false) & 0x3F];
                } else {
                    videoBuffer[scanline * 256 + x] = palScreen[ppuRead(0x3F00, false) & 0x3F];
                }
            }
        }
        
        // Render Sprites Scanline
        if (mask & 0x10) {
            uint16_t sprite_pattern_table = (control & 0x08) ? 0x1000 : 0x0000;
            uint8_t sprite_size = (control & 0x20) ? 16 : 8;
            
            for (int i = 63; i >= 0; i--) {
                sObjectAttributeEntry oam = OAM[i];
                if (oam.y > 0 && scanline >= oam.y && scanline < oam.y + sprite_size) {
                    uint8_t sprite_y = scanline - oam.y;
                    uint8_t tile_id = oam.id;
                    
                    if (oam.attribute & 0x80) sprite_y = (sprite_size - 1) - sprite_y; // Flip V
                    
                    uint16_t sprite_addr = sprite_pattern_table + (tile_id * 16) + sprite_y;
                    uint8_t tile_lsb = ppuRead(sprite_addr, false);
                    uint8_t tile_msb = ppuRead(sprite_addr + 8, false);
                    
                    for (int px = 0; px < 8; px++) {
                        int x_pos = oam.x + px;
                        if (x_pos >= 0 && x_pos < 256) {
                            uint8_t pixel_col = (oam.attribute & 0x40) ? px : (7 - px); // Flip H
                            uint8_t pixel = ((tile_lsb >> pixel_col) & 0x01) | (((tile_msb >> pixel_col) & 0x01) << 1);
                            
                            if (pixel != 0) {
                                uint8_t palette = (oam.attribute & 0x03) + 4;
                                videoBuffer[scanline * 256 + x_pos] = palScreen[ppuRead(0x3F00 + (palette << 2) + pixel, false) & 0x3F];
                                
                                // Fake Sprite 0 Hit calculation (extremely simplified)
                                if (i == 0) {
                                    status |= 0x40; // Set Sprite 0 Hit
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        if (scanline >= 261) {
            scanline = -1;
        }
    }

    if (scanline == 241 && cycle == 1) {
        status |= 0x80; // Set VBlank
        if (control & 0x80) {
            nmi = true; // Trigger NMI
        }
    }

    if (scanline == -1 && cycle == 1) {
        status &= ~0x80; // Clear VBlank
        status &= ~0x40; // Clear Sprite 0 Hit
        if (mask & 0x18) {
            vram_addr = tram_addr; // Reload scrolling / nametable addresses
        }
    }
}

std::array<uint32_t, 128 * 128> PPU2C02::GetPatternTable(uint8_t i, uint8_t palette) {
    std::array<uint32_t, 128 * 128> sprite;
    sprite.fill(0xFF000000);

    for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
        for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
            uint16_t nOffset = nTileY * 256 + nTileX * 16;

            for (uint16_t row = 0; row < 8; row++) {
                uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0x0000, false);
                uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 0x0008, false);

                for (uint16_t col = 0; col < 8; col++) {
                    uint8_t pixel = (tile_lsb & 0x01) + ((tile_msb & 0x01) << 1);
                    tile_lsb >>= 1; tile_msb >>= 1;

                    uint32_t color = 0xFF000000;
                    if (pixel == 1) color = 0xFF555555;
                    else if (pixel == 2) color = 0xFFAAAAAA;
                    else if (pixel == 3) color = 0xFFFFFFFF;

                    int px = nTileX * 8 + (7 - col);
                    int py = nTileY * 8 + row;
                    sprite[py * 128 + px] = color;
                }
            }
        }
    }
    return sprite;
}
