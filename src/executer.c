#include "../include/include.h"

extern int16_t current_instruction;
extern int8_t opcode;
extern int8_t R1;
extern int8_t R2_imm;

void fetch() {
    current_instruction = instruction_memory[PC++];
}

void decode() {
    opcode = current_instruction >> 12;

    R1 = current_instruction >> 6;
    R1 &= ((1 << 7) - 1);

    R2_imm = current_instruction & ((1 << 7) - 1);
}

void execute() {
    
}


void flush() {
    current_instruction = -1;
}