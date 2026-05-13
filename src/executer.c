#include "../include/include.h"

extern int16_t current_instruction;

void fetch() {
    current_instruction = instruction_memory[PC++];
}

void decode() {
    int8_t opcode = current_instruction >> 12;

    int8_t R1 = current_instruction >> 6;
    R1 &= ((1 << 7) - 1);

    int8_t R2 = current_instruction & ((1 << 7) - 1);

    switch(opcode) {
        case ADD:
        case SUB:
        case MUL:
        case LDI:
        case BEQZ:
        case AND:
        case OR:
        case JR:
        case SAL:
        case SAR:
        case LB:
        case SB:
    }

}

void execute_R(int opcode, int R1, int R2) {

}

void execute_I(int opcode, int R1, int imm) {

}


void flush() {
    current_instruction = -1;
}