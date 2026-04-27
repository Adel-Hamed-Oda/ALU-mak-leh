#include "includes.h"
#include "instruction_memory.h"

static instruction_memory_word memory[NUMBER_OF_INSTRUCTION_MEMORY_WORDS];

void initialize_instruction_memory() {
    for (int i = 0; i < NUMBER_OF_INSTRUCTION_MEMORY_WORDS; i++) {
        memory[i].value = 0;
    }
}

int fetch_instruction(int index);
// DESTRUCTIVE, USE WITH CAUTION
int set_instruction(int index, int value);

void print_instruction_memory() {
    printf("Instruction Memory:\n");
    for (int i = 0; i < NUMBER_OF_INSTRUCTION_MEMORY_WORDS; i++) {
        printf("Address %d: Value: %d\n", i, memory[i].value);
    }
}

void print_instruction_memory_l(int start, int end) {
    printf("Instruction Memory:\n");
    for (int i = start; i <= end; i++) {
        printf("Address %d: Value: %d\n", i, memory[i].value);
    }
}