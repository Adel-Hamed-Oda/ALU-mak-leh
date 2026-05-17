#include "../include/include.h"

extern int16_t current_instruction;
extern OPCODE opcode;
extern int8_t R1;
extern int8_t R2_imm;
extern int8_t SREG;
extern int clk;

void to_bin(int16_t x, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        int bit = (x >> i) & 1;
        printf("%i", bit);
    }
    printf("\n");
}

void fetch() {
    if (get_instr(PC) == -1 || PC >= INSTRUCTION_MEMORY_SIZE) {
        current_instruction = -1;
        return;
    }

    printf("[Cycle: %i]: FETCHING THE CURRENT INSTRUCTION AT ADDRESS: %i\n", clk, PC); 

    current_instruction = instruction_memory[PC++];
    printf("[Cycle: %i]: INSTRUCTION = ", clk, current_instruction);
    to_bin(current_instruction, 16);

}

void decode() {
    if (current_instruction == -1) {
        opcode = MEOW;
        R1 = R2_imm = -1;
        return;
    }

    printf("[Cycle: %i]: DECODING THE CURRENT INSTRUCTION\n", clk);
    opcode = ((uint16_t)current_instruction >> 12);

    R1 = current_instruction >> 6;
    R1 &= ((1 << 6) - 1);

    R2_imm = current_instruction & ((1 << 6) - 1);

    printf("[Cycle: %i]: DECODED: OPCODE = %i, R1 = %i, R2/imm = ", clk, opcode, R1);
    to_bin(R2_imm, 6);
}

void execute() {
    SREG = 0;

    if (opcode == MEOW) {
        return;
    }
    printf("[Cycle: %i]: EXECUTING THE CURRENT INSTRUCTION, opcode = %i, R1 = %i, R2 = ", clk, opcode, R1);
    to_bin(R2_imm, 6);

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

    printf("[Cycle: %i]: FLUSH CALLED, IF and ID have been set to NULL\n", clk);
}
