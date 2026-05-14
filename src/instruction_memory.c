#include "../include/include.h"

int cur = 0;
int16_t instruction_memory[INSTRUCTION_MEMORY_SIZE] = {0};

int16_t get_instr(int i) {
    return instruction_memory[i];
}

void push_back_instr(int16_t val) {
    instruction_memory[cur++] = val;
}

void clear_instr() {
    cur = 0;
}

