#include "includes.h"
#include "instruction_memory.h"

static instruction_memory_word memory[NUMBER_OF_INSTRUCTION_MEMORY_WORDS];

static void print_binary_16(int value) {
    for (int i = 15; i >= 0; i--) {
        putchar((value >> i) & 1 ? '1' : '0');
        if (i == 12 || i == 6) putchar('_'); // split into opcode|r1|r2 fields
    }
}

void initialize_instruction_memory() {
    for (int i = 0; i < NUMBER_OF_INSTRUCTION_MEMORY_WORDS; i++) {
        memory[i].value = 0;
    }
}

EXCEPTION fetch_instruction(int index, int *out_value) {
    if (index < 0 || index >= NUMBER_OF_INSTRUCTION_MEMORY_WORDS) {
        return OUT_OF_BOUNDS;
    }
    *out_value = memory[index].value;
    return SUCCESS;
}

EXCEPTION set_instruction(int index, int value) {
    if (index < 0 || index >= NUMBER_OF_INSTRUCTION_MEMORY_WORDS) {
        return OUT_OF_BOUNDS;
    }
    memory[index].value = value;
    return SUCCESS;
}

void clear_instruction_memory() {
    for (int i = 0; i < NUMBER_OF_INSTRUCTION_MEMORY_WORDS; i++) {
        memory[i].value = 0;
    }
}

void print_instruction_memory() {
    printf("Instruction Memory:\n");
    for (int i = 0; i < NUMBER_OF_INSTRUCTION_MEMORY_WORDS; i++) {
        printf("Address %2d: ", i);
        print_binary_16(memory[i].value);
        putchar('\n');
    }
}

void print_instruction_memory_l(int start, int end) {
    printf("Instruction Memory:\n");
    for (int i = start; i <= end; i++) {
        printf("Address %2d: ", i);
        print_binary_16(memory[i].value);
        putchar('\n');
    }
}