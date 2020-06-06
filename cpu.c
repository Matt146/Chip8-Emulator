#include "cpu.h"

cpu_t* init_cpu() {
    // Allocate memory on heap to store CPU
    cpu_t* cpu = (cpu_t*)malloc(sizeof(cpu_t));

    // Initialize memory
    cpu->memory = (unsigned char*)malloc(sizeof(unsigned char) * memory_size);
    cpu->memory_len = memory_size;
    for (size_t i = 0; i < cpu->memory_len; i++) {
        cpu->memory[i] = '\0';
    }

    // Initialize the registers;
    unsigned short subroutine_nesting = 0;

    //  return the CPU
    return cpu;
}

void free_cpu(cpu_t* cpu) {
    free(cpu->memory);
    free(cpu);
}

void cpu_load_program(cpu_t* cpu, const char* fname) {
    // Copy the data of the program into a buffer
    unsigned char* fdata = read_file_binary(fname);
    if (fdata == NULL) {
        Log("Unable to load program into memory!", 3);
        exit(-1);
    }

    // Check if the size of the program data is
    // appropriate (ie: not too large)
    if (strnlen(fdata, max_file_size) > max_program_size) {
        Log("Program too large! Bllaarrggghh (make sure it is < 1024 bytes)", 3);
        exit(-1);
    }

    // Now, copy fdata into the actual memory
    // and free fdata
    memcpy(cpu->memory + 0x200, fdata, strnlen(fdata, max_program_size));
    cpu->memory[0x200 + strnlen(fdata, max_program_size) + 1] = '\0';

    // Finally, set the appropriate registers and free fdata
    cpu->program_len = strnlen(fdata, max_program_size);
    cpu->pc = 0x200;
    free(fdata);
}

void cpu_render_to_screen(SDL_Renderer* renderer, cpu_t* cpu) {
    for (size_t i = 0; i < 32; i++) {
        for (size_t j = 0; j < 64; j++) {
            if (cpu->vram[i][j] == true) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect rect = {i*x_window_scale, j * x_window_scale, x_window_scale, y_window_scale};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}