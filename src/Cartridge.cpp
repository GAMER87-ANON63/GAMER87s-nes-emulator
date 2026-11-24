#include "Cartridge.h"
#include "Mapper_000.h"
#include <fstream>
#include <iostream>

Cartridge::Cartridge(const std::string& sFileName) {
    struct sHeader {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1;
        uint8_t mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1;
        uint8_t tv_system2;
        char unused[5];
    } header;

    std::ifstream ifs(sFileName, std::ifstream::binary);
    if (ifs.is_open()) {
        ifs.read((char*)&header, sizeof(sHeader));

        // Skip trainer if present
        if (header.mapper1 & 0x04)
            ifs.seekg(512, std::ios_base::cur);

        nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);

        // File format type
        uint8_t nFileType = 1;
        if ((header.mapper2 & 0x0C) == 0x08) nFileType = 2;

        if (nFileType == 1) {
            nPRGBanks = header.prg_rom_chunks;
            vPRGMemory.resize(nPRGBanks * 16384);
            ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

            nCHRBanks = header.chr_rom_chunks;
            if (nCHRBanks == 0) {
                // Create CHR RAM
                vCHRMemory.resize(8192);
            } else {
                // Allocate for ROM
                vCHRMemory.resize(nCHRBanks * 8192);
                ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
            }
        } else if (nFileType == 2) {
            // NES 2.0 format not fully supported here, but handle basically
            nPRGBanks = ((header.prg_ram_size & 0x0F) << 8) | header.prg_rom_chunks;
            vPRGMemory.resize(nPRGBanks * 16384);
            ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

            nCHRBanks = ((header.prg_ram_size & 0xF0) << 4) | header.chr_rom_chunks;
            if (nCHRBanks == 0) {
                vCHRMemory.resize(8192);
            } else {
                vCHRMemory.resize(nCHRBanks * 8192);
                ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
            }
        }

        ifs.close();

        // Load correct mapper
        switch (nMapperID) {
            case 0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
        }
    }
}

Cartridge::~Cartridge() {
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data) {
    uint32_t mapped_addr = 0;
    if (pMapper && pMapper->cpuMapRead(addr, mapped_addr)) {
        data = vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped_addr = 0;
    if (pMapper && pMapper->cpuMapWrite(addr, mapped_addr)) {
        vPRGMemory[mapped_addr] = data;
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data) {
    uint32_t mapped_addr = 0;
    if (pMapper && pMapper->ppuMapRead(addr, mapped_addr)) {
        data = vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped_addr = 0;
    if (pMapper && pMapper->ppuMapWrite(addr, mapped_addr)) {
        vCHRMemory[mapped_addr] = data;
        return true;
    }
    return false;
}
