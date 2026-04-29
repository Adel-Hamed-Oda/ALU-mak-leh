#include "includes.h"
#include "register.h"

ca_register create_register(int RegisterType);

static ca_register PC;
static ca_register SREG;
static ca_register GPRS[NUMBER_OF_GP_REGISTERS];

void initialize_registers() {
    PC = create_register(REGISTER_TYPE_PC);
    SREG = create_register(REGISTER_TYPE_STATUS);

    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        GPRS[i] = create_register(REGISTER_TYPE_GP);
    }
}

ca_register create_register(int RegisterType) {
    ca_register reg;
    reg.content = 0;
    reg.register_type = RegisterType;
    return reg;
}

void print_registers() {
    printf("PC Register: Content: %d\n", PC.content);
    printf("Status Register: Content: %d\n", SREG.content);
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        printf("GP Register %d: Content: %d\n", i, GPRS[i].content);
    }
}

void print_register(int index) {
    if (index == 0) {
        printf("PC Register: Content: %d\n", PC.content);
    } else if (index == 1) {
        printf("Status Register: Content: %d\n", SREG.content);
    } else {
        for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
            if (index == i) {
                printf("GP Register %d: Content: %d\n", i, GPRS[i].content);
                return;
            }
        }
        printf("Register with index %d not found.\n", index);
    }
}