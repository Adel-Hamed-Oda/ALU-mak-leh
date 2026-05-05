#include "includes.h"
#include "data_memory.h"

static data_memory_word memory[NUMBER_OF_DATA_MEMORY_WORDS];

void initialize_data_memory() {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        memory[i].value = 0;
    }
}

EXCEPTION get_data(int index, int *out_value);
EXCEPTION set_data(int index, int value);
EXCEPTION get_index(char *name, int *out_index);

void clear_data_memory() {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        memory[i].value = 0;
    }
}

void print_data_memory() {
    printf("Data Memory:\n");
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        printf("Address %d: Value: %d\n", i, memory[i].value);
    }
}
void print_data_memory_l(int start, int end) {
    printf("Data Memory:\n");
    for (int i = start; i <= end; i++) {
        printf("Address %d: Value: %d\n", i, memory[i].value);
    }
}