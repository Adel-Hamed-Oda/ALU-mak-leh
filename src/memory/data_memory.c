#include "includes.h"
#include "memory_words.h"

static data_memory_word memory[NUMBER_OF_DATA_MEMORY_WORDS];

void initialize_data_memory() {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        memory[i].name = NULL;
        memory[i].value = 0;
    }
}

// DESTRUCTIVE, USE WITH CAUTION
int set_value_at_n(char *name, int value) {
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        if (strcmp(memory[i].name, name) == 0) {
            memory[i].value = value;
            return 0;
        }
    }

    return -1;
}
// DESTRUCTIVE, USE WITH CAUTION
int set_value_at_index(int index, int value) {
    if (index < 0 || index >= NUMBER_OF_DATA_MEMORY_WORDS) {
        return -1; // Out of bounds
    }
    memory[index].value = value;

    return 0;
}

void print_data_memory() {
    printf("Data Memory:\n");
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        printf("Address %d: Name: %s, Value: %d\n", i, memory[i].name, memory[i].value);
    }
}
void print_data_memory_l(int start, int end) {
    printf("Data Memory:\n");
    for (int i = start; i <= end; i++) {
        printf("Address %d: Name: %s, Value: %d\n", i, memory[i].name, memory[i].value);
    }
}