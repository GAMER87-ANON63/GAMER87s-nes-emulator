#include <iostream>
#include <memory>
#include <SDL2/SDL.h>
#include "Bus.h"
#include "CPU6502.h"
#include "Cartridge.h"

int main(int argc, char* argv[]) {
    std::cout << "NES Emulator starting..." << std::endl;

    Bus bus;
    CPU6502 cpu;
    
    cpu.ConnectBus(&bus);

    if (argc > 1) {
        auto cart = std::make_shared<Cartridge>(argv[1]);
        bus.insertCartridge(cart);
        std::cout << "Loaded Cartridge: " << argv[1] << std::endl;
        cpu.reset();
    } else {
        std::cout << "Usage: nes_emu <rom_file.nes>" << std::endl;
        std::cout << "Starting without a cart..." << std::endl;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256 * 3, 240 * 3, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }

        // Map Keyboard to Controller 0
        bus.controller[0] = 0x00;
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_X])     bus.controller[0] |= 0x80; // A
        if (state[SDL_SCANCODE_Z])     bus.controller[0] |= 0x40; // B
        if (state[SDL_SCANCODE_A])     bus.controller[0] |= 0x20; // Select
        if (state[SDL_SCANCODE_S])     bus.controller[0] |= 0x10; // Start
        if (state[SDL_SCANCODE_UP])    bus.controller[0] |= 0x08; // Up
        if (state[SDL_SCANCODE_DOWN])  bus.controller[0] |= 0x04; // Down
        if (state[SDL_SCANCODE_LEFT])  bus.controller[0] |= 0x02; // Left
        if (state[SDL_SCANCODE_RIGHT]) bus.controller[0] |= 0x01; // Right

        // Emulate 1 frame
        do {
            bus.ppu.clock();
            if (bus.nSystemClockCounter % 3 == 0) {
                if (bus.ppu.nmi) {
                    bus.ppu.nmi = false;
                    cpu.nmi();
                }
                cpu.clock();
            }
            bus.nSystemClockCounter++;
        } while (!(bus.ppu.scanline == 241 && bus.ppu.cycle == 0));

        SDL_UpdateTexture(texture, nullptr, bus.ppu.videoBuffer.data(), 256 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
