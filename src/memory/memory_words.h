#include "includes.h"

typedef struct {
    short value;
} instruction_memory_word;

typedef struct {
    char *name; // I think we will use name for variable references
    uint8_t value;
} data_memory_word;