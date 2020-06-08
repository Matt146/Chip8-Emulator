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
    unsigned short  pc;             // program counter (instruction pointer)
    unsigned short stack[16];
    unsigned short sp;

    // Video Memory
    bool vram[32][64];

    // General Registers
    unsigned char   reg[16];
    unsigned short  I;

    // Time & Sound Registers
    // Chip-8 specifies two 8-bit registers for delay and sound
    unsigned char   time_delay;
    unsigned char   sound_delay;

    // IO buffer - not part of chip8 spec but kinda a hack
    // for keyboard input
    char io_buff[256];

    // SDL event handler
    SDL_Event ev;
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

// cpu_render_to_screen - this renders everything in vram
// to the screen
void cpu_render_to_screen(SDL_Renderer* renderer, cpu_t* cpu);

// cpu_log_io - this takes a keyboard input and puts in into the cpu io buffer
void cpu_log_io(cpu_t* cpu, char input);

// cpu_flush_io_buffer - this takes the cpu io buffer and
// just sets every input to -127, essentially clearing it out
void cpu_flush_io_buffer(cpu_t* cpu);


/********************************************************************
 * The following functions are responsible for handling instructions
********************************************************************/
void cpu_instr_cls(cpu_t* cpu);
void cpu_instr_ret(cpu_t* cpu);
void cpu_instr_jp(cpu_t* cpu, unsigned short addr);
void cpu_instr_call(cpu_t* cpu, unsigned short addr);
void cpu_instr_se(cpu_t* cpu, unsigned char reg, unsigned char byte);
void cpu_instr_sne(cpu_t* cpu, unsigned char reg, unsigned char byte);
void cpu_instr_seregreg(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_ld(cpu_t* cpu, unsigned char reg, unsigned char byte);
void cpu_instr_add(cpu_t* cpu, unsigned char reg, unsigned char byte);
void cpu_instr_regreg(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_or(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_and(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_xor(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_addcarry(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_sub(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_shr(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_subn(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_snenotequal(cpu_t* cpu, unsigned char reg1, unsigned char reg2);
void cpu_instr_a(cpu_t* cpu, unsigned short value);
void cpu_instr_b(cpu_t* cpu, unsigned short addr);
void cpu_instr_c(cpu_t* cpu, unsigned char reg);
void cpu_instr_d(cpu_t* cpu, unsigned char reg1, unsigned char reg2, unsigned char n);
void cpu_instr_skp(cpu_t* cpu, unsigned char reg1);
void cpu_instr_sknp(cpu_t* cpu, unsigned char reg1);
void cpu_instr_lddt(cpu_t* cpu, unsigned char reg);
void cpu_instr_ldio(cpu_t* cpu, unsigned char reg);
void cpu_instr_lddt1(cpu_t* cpu, unsigned char reg);

// cpu_emulate - this causes one emulation cycle
// (fetch, decode, execute)
void cpu_emulate(cpu_t* cpu);

#endif // CPU_H