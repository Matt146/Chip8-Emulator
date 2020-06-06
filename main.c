#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include "cpu.h"

int main(int argc, char** argv) {
    // Check if we have valid arguments
    if (argc < 2) {
        Log("Incorrect usage!", 3);
        printf("\tCorrect usage: ./a.out <program file name>\n");
        return -1;
    }
    
    // Set up SDL Window
    SDL_Window* window = NULL;
    window = SDL_CreateWindow("Chip8",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            64*x_window_scale,
                            32*y_window_scale,
                            SDL_WINDOW_OPENGL);
    if (window == NULL) {
        Log("Unable to create SDL window!", 3);
        return -1;
    }

    // Set up SDL Renderer
    SDL_Renderer* renderer = NULL;
    renderer = SDL_CreateRenderer(window, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initialize CPU, memory, and registers
    Log("Initializing CPU, memory, and registers...", 0);
    cpu_t* cpu = init_cpu();
    Log("Successfully initialized!", 0);

    // Load the program
    Log("Loading program...", 0);
    cpu_load_program(cpu, argv[1]);
    Log("Program loaded!", 0);

    // Test Graphics
    for (size_t i = 0; i < 32; i++) {
        for (size_t j = 0; j < 64; j++) {
            cpu->vram[i][j] = false;
            if (i == 2 && j == 2) {
                cpu->vram[i][j] = true;
            } else if (i == 2 && j == 3) {
                cpu->vram[i][j] = true;
            } else if (i == 3 && j == 4) {
                cpu->vram[i][j] = true;
            }
        }
    }

    // Mainloop
    bool running = true;
    SDL_Event ev;
    while (running == true) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_KEYDOWN:
                    if (ev.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                case SDL_QUIT:
                    running = false;
            }
        }

        // Loop operations here

        // Render here
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_Rect rect = {0, 0, x_window_scale * 64, y_window_scale * 32};
        SDL_RenderFillRect(renderer, &rect);
        cpu_render_to_screen(renderer, cpu);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    Log("Cleaning up...", 0);
    free_cpu(cpu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}