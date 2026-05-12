#include "includes.h"
#include "data_memory.h"

static data_memory_word memory[NUMBER_OF_DATA_MEMORY_WORDS];

void initialize_data_memory(void) {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++)
        memory[i].value = 0;
}

void clear_data_memory(void) {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++)
        memory[i].value = 0;
}

EXCEPTION get_data(int index, int *out_value) {
    if (index < 0 || index >= NUMBER_OF_DATA_MEMORY_WORDS)
        return OUT_OF_BOUNDS;
    *out_value = memory[index].value;
    return SUCCESS;
}

EXCEPTION set_data(int index, int value) {
    if (index < 0 || index >= NUMBER_OF_DATA_MEMORY_WORDS)
        return OUT_OF_BOUNDS;
    memory[index].value = value;
    return SUCCESS;
}

void print_data_memory(void) {
    printf("=== Final Data Memory ===\n");
    int any_nonzero = 0;
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        if (memory[i].value != 0) {
            printf("[%4d]: %d\n", i, memory[i].value);
            any_nonzero = 1;
        }
    }
    if (!any_nonzero)
        printf("(all zeros)\n");
}

void print_data_memory_l(int start, int end) {
    printf("Data Memory [%d..%d]:\n", start, end);
    for (int i = start; i <= end; i++)
        printf("[%4d]: %d\n", i, memory[i].value);
}
