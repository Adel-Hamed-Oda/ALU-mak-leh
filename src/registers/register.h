#include "includes.h"

#define REGISTER_TYPE_PC 0
#define REGISTER_TYPE_STATUS 1
#define REGISTER_TYPE_GP 2

typedef struct {
    int index;
    uint8_t content;
    int register_type;
} ca_register;

void initialize_registers();
void print_registers();
void print_register(int index);