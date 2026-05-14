#include "../include/include.h"

extern int16_t current_instruction;
extern int8_t opcode;
extern int8_t R1;
extern int8_t R2_imm;
extern int clk;

void fetch() {
    printf("[Cycle: %i]: FETCHING THE CURRENT INSTRUCTION AT ADDRESS %i\n", clk, PC);
    current_instruction = instruction_memory[PC++];
}

void decode() {
    if (current_instruction == -1) {
        return;
    }

    printf("[Cycle: %i]: DECODING THE CURRENT INSTRUCTION\n", clk);
    opcode = current_instruction >> 12;

    R1 = current_instruction >> 6;
    R1 &= ((1 << 6) - 1);

    R2_imm = current_instruction & ((1 << 6) - 1);
}

void execute() {
    if (opcode == MEOW) {
        return;
    }
    printf("[Cycle: %i]: EXECUTING THE CURRENT INSTRUCTION\n", clk);

    switch(opcode) {
        case ADD: {
            add();
            break;
        }
        case SUB: {
            sub();
            break;
        }
        case MUL: {
            mul();
            break;
        }
        case LDI: {
            ldi();
            break;
        }
        case BEQZ: {
            beqz();
            break;
        }
        case AND: {
            and();
            break;
        }
        case OR: {
            or();
            break;
        }
        case JR: {
            jr();
            break;
        }
        case SAL: {
            sal();
            break;
        }
        case SAR: {
            sar();
            break;
        }
        case LB: {
            lb();
            break;
        }
        case SB: {
            sb();
            break;
        }
    }
}


void flush() {
    current_instruction = -1;
    opcode = MEOW;
}
