

#include "includes.h"

#define REGISTER_TYPE_GP 0
#define REGISTER_TYPE_STATUS 1
#define REGISTER_TYPE_PC 2

typedef struct {
    int index;
    uint8_t content;
    int register_type;
} ca_register;