#include "include.h"

int8_t get_data(int i) {
    return data_memory[i];
}

void set_data(int i, int8_t x) {
    data_memory[i] = x;
}

