#pragma once

#include "types.h"
#include <stdint.h>

extern int cur;
extern int16_t instruction_memory[INSTRUCTION_MEMORY_SIZE];

int16_t get_instr(int i);
void push_back_instr(int16_t inst);
void clear_instr();
