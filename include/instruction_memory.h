#include "types.h"
#include "stdint.h"

int cur;
int16_t instruction_memory[INSTRUCTION_MEMORY_SIZE];

int16_t get_instr(int i);
void push_back_instr();
void clear_instr();
