#include "register.h"

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