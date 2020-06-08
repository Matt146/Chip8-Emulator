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

    // Initialize io buffer
    for (size_t i = 0; i < sizeof(cpu->io_buff) / sizeof(char); i++) {
        cpu->io_buff[i] = -127;
    }

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

void cpu_log_io(cpu_t* cpu, char input) {
    int array_size = sizeof(cpu->io_buff) / sizeof(char);
    if (array_size < 256) {
        cpu->io_buff[array_size-1] = input;
    }
}

void cpu_flush_io_buffer(cpu_t* cpu) {
    for (size_t i = 0; i < sizeof(cpu->io_buff) / sizeof(char); i++) {
        cpu->io_buff[i] = -127;
    }
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

void cpu_instr_cls(cpu_t* cpu) {
    for (size_t i = 0; i < 32; i++) {
        for (size_t j = 0; j < 64; j++) {
            cpu->vram[i][j] == false;
        }
    }
}

void cpu_instr_ret(cpu_t* cpu) {
    cpu->pc = cpu->stack[cpu->sp];
    cpu->sp -= 1;
}

void cpu_instr_jp(cpu_t* cpu, unsigned short addr) {
    cpu->pc = addr;
}

void cpu_instr_call(cpu_t* cpu, unsigned short addr) {
    cpu->sp += 1;
    cpu->stack[cpu->sp] = cpu->pc;
    cpu->pc = addr;
}

void cpu_instr_se(cpu_t* cpu, unsigned char reg, unsigned char byte) {
    if (cpu->reg[reg] == byte) {
        cpu->pc += 2;
    }
}

void cpu_instr_sne(cpu_t* cpu, unsigned char reg, unsigned char byte) {
    if (cpu->reg[reg] != byte) {
        cpu->pc += 2;
    }
}

void cpu_instr_seregreg(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg1] == cpu->reg[reg2]) {
        cpu->pc += 2;
    }
}

void cpu_instr_ld(cpu_t* cpu, unsigned char reg, unsigned char byte) {
    cpu->reg[reg] = byte;
}

void cpu_instr_add(cpu_t* cpu, unsigned char reg, unsigned char byte) {
    cpu->reg[reg] = cpu->reg[reg] + byte;
}

void cpu_instr_regreg(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    cpu->reg[reg1] = cpu->reg[reg2];
}

void cpu_inst_or(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    cpu->reg[reg1] = cpu->reg[reg1] | cpu->reg[reg2];
}

void cpu_instr_and(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    cpu->reg[reg1] = cpu->reg[reg1] & cpu->reg[reg2];
}

void cpu_instr_xor(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    cpu->reg[reg1] = cpu->reg[reg1] ^ cpu->reg[reg2];
}

void cpu_instr_addcarry(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    unsigned short sum = cpu->reg[reg1] + cpu->reg[reg2];
    unsigned char carry = sum >> 8;
    unsigned char sum1 = sum & 0x0ff;
    cpu->reg[reg1] = sum1;
    cpu->reg[reg2] = carry;
}

void cpu_instr_sub(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg1] > cpu->reg[reg2]) {
        cpu->reg[15] = 1;
    } else {
        cpu->reg[15] = 0;
    }
    cpu->reg[reg1] = cpu->reg[reg1] - cpu->reg[reg2];
}

void cpu_instr_shr(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg1] & 0x0f == 0x1) {
        cpu->reg[15] = 1;
    } else {
        cpu->reg[15] = 0;
    }
    cpu->reg[reg1] /= 2;
}

void cpu_instr_subn(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg2] > cpu->reg[reg1]) {
        cpu->reg[15] = 1;
    } else {
        cpu->reg[15] = 0;
    }
    cpu->reg[reg1] = cpu->reg[reg2] - cpu->reg[reg1];
}

void cpu_instr_shl(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg1] >> 7 == 0x1) {
        cpu->reg[15] = 1;
    } else {
        cpu->reg[15] = 0;
    }
    cpu->reg[reg1] *= 2;
}

void cpu_instr_snenotequal(cpu_t* cpu, unsigned char reg1, unsigned char reg2) {
    if (cpu->reg[reg1] != cpu->reg[reg2]) {
        cpu->pc += 2;
    }
}

void cpu_instr_a(cpu_t* cpu, unsigned short value) {
    cpu->I = value;
}

void cpu_instr_b(cpu_t* cpu, unsigned short addr) {
    cpu->pc = addr + cpu->reg[0];
}

void cpu_instr_c(cpu_t* cpu, unsigned char reg) {
    unsigned char rand_byte = rand() % 255;
    cpu->reg[reg] = rand_byte;
}

void cpu_instr_d(cpu_t* cpu, unsigned char reg1, unsigned char reg2, unsigned char n) {
    // load the sprite data
    unsigned short sprite_start = cpu->I;
    unsigned char* sprite_data = (unsigned char*)malloc(sizeof(unsigned char) * 18);
    for (size_t i = sprite_start; i < n; i++) {
        sprite_data[i] = cpu->memory[i];
    }

    // actually update vram now
    for (size_t sprite_byte_index = 0; sprite_byte_index < n; sprite_byte_index++) {
        for (size_t j = 0; j < 8; j++) {
            unsigned char bit = sprite_byte_index>>j;
            if (bit == 0x0 && cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] == false) {
                cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] = false;
            } else if (bit == 0x1 || cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] == true) {
                cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] = true;
            } else if (bit == 0x1 && cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] == true) {
                cpu->vram[cpu->reg[reg1]][cpu->reg[reg2]] = false;
            }
        }
    }

    free(sprite_data);
}

void cpu_instr_skp(cpu_t* cpu, unsigned char reg1) {
    for (size_t i = 0; i < sizeof(cpu->io_buff)/sizeof(char); i++) {
        if (cpu->io_buff[i] == cpu->reg[reg1]) {
            cpu->pc += 2;
        }
    }
}

void cpu_instr_sknp(cpu_t* cpu, unsigned char reg1) {
    // detect if the key has been pressed in the io buffer
    bool pressed = false;
    for (size_t i = 0; i < sizeof(cpu->io_buff)/sizeof(char); i++) {
        if (cpu->io_buff[i] == cpu->reg[reg1]) {
            pressed = true;
        }
    }

    // if it hasn't, inc the program counter
    if (pressed == false) {
        cpu->pc += 2;
    }
}

void cpu_instr_lddt(cpu_t* cpu, unsigned char reg) {
    cpu->reg[reg] = cpu->time_delay;
}

void cpu_instr_ldio(cpu_t* cpu, unsigned char reg) {
    while (SDL_PollEvent(&(cpu->ev))) {
        switch (cpu->ev.type) {
            case SDL_KEYDOWN:
                switch (cpu->ev.type) {
                    case SDL_KEYDOWN:
                        switch (cpu->ev.key.keysym.sym) {
                            case SDLK_x:
                                // map to "0" key
                                cpu_log_io(cpu, 0x0);
                                break;
                            case SDLK_1:
                                // map to "1" key
                                cpu_log_io(cpu, 0x1);
                                break;
                            case SDLK_2:
                                // map to "2" key
                                cpu_log_io(cpu, 0x2);
                                break;
                            case SDLK_3:
                                // map to "3" key
                                cpu_log_io(cpu, 0x3);
                                break;
                            case SDLK_4:
                                // map to "c" key
                                cpu_log_io(cpu, 0xc);
                                break;
                            case SDLK_q:
                                // map to "4" key
                                cpu_log_io(cpu, 0x4);
                                break;
                            case SDLK_w:
                                // map to "5" key
                                cpu_log_io(cpu, 0x5);
                                break;
                            case SDLK_e:
                                // map to "6" key
                                cpu_log_io(cpu, 0x6);
                                break;
                            case SDLK_r:
                                // map to "d" key
                                cpu_log_io(cpu, 0xd);
                                break;
                            case SDLK_a:
                                // map to "7" key
                                cpu_log_io(cpu, 0x7);
                                break;
                            case SDLK_s:
                                // map to "8" key
                                cpu_log_io(cpu, 0x8);
                                break;
                            case SDLK_d:
                                // map to "9" key
                                cpu_log_io(cpu, 0x9);
                                break;
                            case SDLK_f:
                                // map to "e" key
                                cpu_log_io(cpu, 0xe);
                                break;
                            case SDLK_z:
                                // map to "a" key
                                cpu_log_io(cpu, 0xa);
                                break;
                            case SDLK_c:
                                // map to "b" key
                                cpu_log_io(cpu, 0xb);
                                break;
                            case SDLK_v:
                                // map to "f" key
                                cpu_log_io(cpu, 0xf);
                                break;
                        }
                }
        }
    }
}

void cpu_instr_lddt1(cpu_t* cpu, unsigned char reg) {
    cpu->time_delay = cpu->reg[reg];
}

void cpu_emulate(cpu_t* cpu) {
    unsigned short instruction = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc+1];
    Log("Emulating!", 0);
    printf("\t0x%x\n", instruction);
    if (instruction == 0x00E0) {
        // CLS instruction
        cpu_instr_cls(cpu);
    } else if (instruction == 0x00EE) {
        // ret instruction - returns from subroutine
        cpu_instr_ret(cpu);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x1) {
        // jp instruction
        printf("Loc: 0x%x\n", cpu->memory[cpu->pc] & 0x0f);
        printf("Loc: 0x%x\n", (cpu->memory[cpu->pc] & 0x0f) << 8);
        unsigned short addr = ((cpu->memory[cpu->pc] & 0x0f) << 8) | cpu->memory[cpu->pc+1];
        printf("Loc: 0x%x\n", addr);
        cpu_instr_jp(cpu, addr);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x2) {
        // call instruction
        unsigned short addr = ((cpu->memory[cpu->pc] & 0x0f) << 8) | cpu->memory[cpu->pc+1];
        cpu_instr_call(cpu, addr);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x3) {
        // skip next instruction (reg-value) equal instruction
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        unsigned char byte = cpu->memory[cpu->pc + 1];
        cpu_instr_se(cpu, reg, byte);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x4) {
        // skip next instruction (reg-value) not equal instruction
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        unsigned char byte = cpu->memory[cpu->pc + 1];
        cpu_instr_sne(cpu, reg, byte);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x5) {
        // skip next instruction (reg-reg) equal instruction
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_seregreg(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x6) {
        // ld instrution; set reg = byte
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        unsigned char byte = cpu->memory[cpu->pc + 1];
        cpu_instr_ld(cpu, reg, byte);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x7) {
        // add instruction; add byte to reg and store result in reg
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        unsigned char byte = cpu->memory[cpu->pc + 1];
        cpu_instr_add(cpu, reg, byte);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x0) {
        // ld instruction; set reg1 equal to reg2
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_regreg(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x1) {
        // OR instruction; set reg1 = reg1 | reg2
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_or(cpu, reg1, reg2);
    } else if (instruction >> 12 == 0x8 && cpu->memory[cpu->pc+1] >> 4 == 0x2) {
        // OR instruction; set reg1 = reg1 | reg2
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_and(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x3) {
        // XOR instruction; set reg1 = reg1 ^ reg2;
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_xor(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x4) {
        // add with carry; reg1 = reg1 + reg2, set reg2 = carry
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_addcarry(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x5) {
        // sub; If Vx > Vy, then VF is set to 1, otherwise 0. 
        // Then Vy is subtracted from Vx, and the results stored in Vx.
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_sub(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x6) {
        // shr; If the least-significant bit of Vx is 1, then VF is set to 1, 
        // otherwise 0. Then Vx is divided by 2.
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_shr(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0x07) {
        // subn
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_subn(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x8 && cpu->memory[cpu->pc+1] & 0x0f == 0xe) {
        // shl
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_shl(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0x9 && cpu->memory[cpu->pc+1] & 0x0f == 0x0) {
        // sne; skip next instruction if vx != vy
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        cpu_instr_snenotequal(cpu, reg1, reg2);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xa) {
        // a; Set I = nnn 
        unsigned short val = cpu->memory[cpu->pc] & 0x0f | cpu->memory[cpu->pc+1];
        cpu_instr_a(cpu, val);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xb) {
        // b; jump to location nnn+V0
        unsigned char addr = ((cpu->memory[cpu->pc] & 0x0f) << 8) | cpu->memory[cpu->pc+1];
        cpu_instr_b(cpu, addr);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xc) {
        // c; set vx = random byte and kk
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_c(cpu, reg);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xd) { 
        // draw instruction
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        unsigned char reg2 = cpu->memory[cpu->pc + 1] >> 4;
        unsigned char n = cpu->memory[cpu->pc + 1] & 0x0f;
        cpu_instr_d(cpu, reg1, reg2, n);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xe && cpu->memory[cpu->pc + 1] == 0x9e) {
        // skp; skip next instruction if key with the value of vx is pressed
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_skp(cpu, reg1);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xe && cpu->memory[cpu->pc + 1] == 0xa1) {
        // sknp; skip next instruction if key with value of vx is NOT pressed
        unsigned char reg1 = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_sknp(cpu, reg1);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xf && cpu->memory[cpu->pc + 1] == 0x07) {
        // set vx = delay timer value
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_lddt(cpu, reg);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xf && cpu->memory[cpu->pc + 1] == 0x0a) {
        // ldio; wait for a key press, store the value in vx
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_ldio(cpu, reg);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xf && cpu->memory[cpu->pc + 1] == 0x15) {
        // set delay timer = vx;
        unsigned char reg = cpu->memory[cpu->pc] & 0x0f;
        cpu_instr_lddt1(cpu, reg);
    } else if (cpu->memory[cpu->pc] >> 4 == 0xf && cpu->memory[cpu->pc + 1] == 0x18) {

    }

    cpu->pc += 2;
    if (cpu->time_delay > 0) {
        cpu->time_delay -= 1;
    }
}
