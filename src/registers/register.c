#include "includes.h"
#include "register.h"

register_8 create_register_8();
register_16 create_register_16();

static register_16 PC;
static register_8 SREG;
static register_8 GPRS[NUMBER_OF_GP_REGISTERS];

#pragma region Initialization Operations

void initialize_registers() {
    PC = create_register_16();
    SREG = create_register_8();

    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        GPRS[i] = create_register_8();
    }
}

register_8 create_register_8() {
    register_8 reg;
    reg.content = 0;
    return reg;
}

register_16 create_register_16() {
    register_16 reg;
    reg.content = 0;
    return reg;
}

#pragma endregion

#pragma region Register Operationss

EXCEPTION set_register(int index, int value) {
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        if (index == i) {
            GPRS[i].content = value & 0xFF; // Ensure only the lower 8 bits are stored
            return SUCCESS;
        }
    }
    return OUT_OF_BOUNDS; // Register index out of bounds
}

EXCEPTION get_register(int index, int *out_value) {
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        if (index == i) {
            *out_value = GPRS[i].content;
            return SUCCESS;
        }
    }
    return OUT_OF_BOUNDS; // Register index out of bounds
}

EXCEPTION clear_register(int index) {
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++) {
        if (index == i) {
            GPRS[i].content = 0;
            return SUCCESS;
        }
    }
    return OUT_OF_BOUNDS; // Register index out of bounds
}

#pragma endregion

#pragma region PC Operations

int get_pc() {
    return PC.content;
}

void set_pc(int new_pc) {
    PC.content = new_pc & 0xFFFF;
}

void increment_pc() {
    PC.content = (PC.content + 1) & 0xFFFF;
}

#pragma endregion

#pragma region Status Register Operations

int get_flag(STATUS_FLAG flag) {
    return (SREG.content & flag) != 0;
}

void update_status_register() {
    // I will leave this till Alaa or whoever is using the parser to implement
    // it cause I need these stuff to be done to be able to update the status register
    // properly, for now I am done
}

#pragma endregion

#pragma region Debugging Operations

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

#pragma endregion