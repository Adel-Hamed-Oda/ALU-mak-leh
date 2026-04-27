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
    static int register_index = 0;
    ca_register reg;
    reg.index = register_index++;
    reg.content = 0;
    reg.register_type = RegisterType;
    return reg;
}

void print_registers() {
    printf("PC Register: Index: %d, Content: %d\n", pc.index, pc.content);
    printf("Status Register: Index: %d, Content: %d\n", status.index, status.content);
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        printf("GP Register %d: Index: %d, Content: %d\n", i, general_purpose[i].index, general_purpose[i].content);
    }
}

void print_register(int index) {
    if (index == pc.index) {
        printf("PC Register: Index: %d, Content: %d\n", pc.index, pc.content);
    } else if (index == status.index) {
        printf("Status Register: Index: %d, Content: %d\n", status.index, status.content);
    } else {
        for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
            if (index == general_purpose[i].index) {
                printf("GP Register %d: Index: %d, Content: %d\n", i, general_purpose[i].index, general_purpose[i].content);
                return;
            }
        }
        printf("Register with index %d not found.\n", index);
    }
}