#include "includes.h"
#include "register.h"

static register_16 PC;
static register_8  SREG;
static register_8  GPRS[NUMBER_OF_GP_REGISTERS];

/* ---------- Initialization ---------- */

void initialize_registers(void) {
    PC.content   = 0;
    SREG.content = 0;
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++)
        GPRS[i].content = 0;
}

/* ---------- GPR Operations ---------- */

EXCEPTION set_register(int index, int value) {
    if (index < 0 || index >= NUMBER_OF_GP_REGISTERS)
        return OUT_OF_BOUNDS;
    GPRS[index].content = (int8_t)(value & 0xFF);
    return SUCCESS;
}

EXCEPTION get_register(int index, int *out_value) {
    if (index < 0 || index >= NUMBER_OF_GP_REGISTERS)
        return OUT_OF_BOUNDS;
    *out_value = GPRS[index].content;
    return SUCCESS;
}

EXCEPTION clear_register(int index) {
    if (index < 0 || index >= NUMBER_OF_GP_REGISTERS)
        return OUT_OF_BOUNDS;
    GPRS[index].content = 0;
    return SUCCESS;
}

/* ---------- PC Operations ---------- */

int get_pc(void) {
    return (int)(PC.content & 0xFFFF);
}

void set_pc(int new_pc) {
    PC.content = (int)(new_pc & 0xFFFF);
}

void increment_pc(void) {
    PC.content = (int)((PC.content + 1) & 0xFFFF);
}

/* ---------- SREG Operations ---------- */

int get_flag(STATUS_FLAG flag) {
    return (SREG.content & (int)flag) != 0;
}

void set_sreg_flag(STATUS_FLAG flag, int value) {
    if (value)
        SREG.content |= (int)flag;
    else
        SREG.content &= ~(int)flag;
    SREG.content &= 0x1F;   /* keep only bits 4:0 */
}

int get_sreg_full(void) {
    return SREG.content & 0x1F;
}

void set_sreg_full(int val) {
    SREG.content = val & 0x1F;
}

/* ---------- Debug ---------- */

void print_registers(void) {
    printf("PC   = %d\n", get_pc());
    printf("SREG = 0x%02X (C=%d, V=%d, N=%d, S=%d, Z=%d)\n",
           get_sreg_full(),
           get_flag(C_FLAG), get_flag(V_FLAG),
           get_flag(N_FLAG), get_flag(S_FLAG), get_flag(Z_FLAG));
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++)
        printf("R%-2d = %d\n", i, GPRS[i].content);
}

void print_register(int index) {
    if (index < 0 || index >= NUMBER_OF_GP_REGISTERS) {
        printf("Register index %d out of range.\n", index);
        return;
    }
    printf("R%d = %d\n", index, GPRS[index].content);
}

void dump_all_registers(void) {
    printf("=== Final Register State ===\n");
    for (int i = 0; i < NUMBER_OF_GP_REGISTERS; i++)
        printf("R%-2d = %d\n", i, GPRS[i].content);
    printf("PC   = %d\n", get_pc());
    printf("SREG = 0x%02X (C=%d, V=%d, N=%d, S=%d, Z=%d)\n",
           get_sreg_full(),
           get_flag(C_FLAG), get_flag(V_FLAG),
           get_flag(N_FLAG), get_flag(S_FLAG), get_flag(Z_FLAG));
}
