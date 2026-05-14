#pragma once

#include "types.h"
#include <stdint.h>

extern int8_t data_memory[DATA_MEMORY_SIZE];


int8_t get_data(int i);
void set_data(int i, int8_t x);
