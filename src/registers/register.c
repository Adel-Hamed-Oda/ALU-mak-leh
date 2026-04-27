#include "includes.h"
#include "register.h"

ca_register create_register(int RegisterType);

static ca_register pc;
static ca_register status;
static ca_register general_purpose[NUMBER_OF_GP_REGISTERS];

void initialize_registers() {
    pc = create_register(REGISTER_TYPE_PC);
    status = create_register(REGISTER_TYPE_STATUS);

    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        general_purpose[i] = create_register(REGISTER_TYPE_GP);
    }
}

ca_register create_register(int RegisterType) {
    ca_register reg;
    reg.content = 0;
    reg.register_type = RegisterType;
    return reg;
}

void print_registers() {
    printf("PC Register: Content: %d\n", pc.content);
    printf("Status Register: Content: %d\n", status.content);
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        printf("GP Register %d: Content: %d\n", i, general_purpose[i].content);
    }
}

void print_register(int index) {
    if (index == 0) {
        printf("PC Register: Content: %d\n", pc.content);
    } else if (index == 1) {
        printf("Status Register: Content: %d\n", status.content);
    } else {
        for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
            if (index == i) {
                printf("GP Register %d: Content: %d\n", i, general_purpose[i].content);
                return;
            }
        }
        printf("Register with index %d not found.\n", index);
    }
}