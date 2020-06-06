#ifndef CPU_H
#define CPU_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

typedef struct CPU {
    // Memory
    unsigned char*  memory;
    int             memory_len;
    int             program_len;    // this is the length of the program

    // Video Memory
    bool vram[32][64];

    // General Registers
    unsigned char   reg[16];
    unsigned short  I;

    // Time & Sound Registers
    // Chip-8 specifies two 8-bit registers for delay and sound
    unsigned char   ts1;
    unsigned char   ts2;

    // Psuedo-registers (not accessible from Chip-8 programs)
    unsigned short  pc;     // program counter (instruction pointer)
    unsigned short  sp;     // stack pointer
    unsigned short  bp;     // base pointer (not sure if I need it)

    // Some utility stuff
    unsigned short subroutine_nesting;   // Chip-8 only allows 16 levels of nested subroutines
} cpu_t;

// init_cpu - use this to initialize a cpu
// "object." almost all cpu-related functions
// require the cpu pointer to not be null, otherwise
// a segmentation fault may occur
cpu_t* init_cpu();

// free_cpu - use this function when cleaning up to prevent
// memory leaks
void free_cpu();

// cpu_load_program - this loads a regular Chip-8 program
// Currently, I don't have support for ETI 660
void cpu_load_program(cpu_t* cpu, const char* fname);
void cpu_render_to_screen(SDL_Renderer* renderer, cpu_t* cpu);

#endif // CPU_H