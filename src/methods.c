#include "../include/include.h"

#define C_FLAG 4
#define V_FLAG 3
#define N_FLAG 2
#define S_FLAG 1
#define Z_FLAG 0

void set(int b)
{
    SREG |= (1 << b);
}
void reset(int b)
{
    SREG &= ~(1 << b);
}

void add()
{
    // Casing into uint8_t to force bitwise adding and use the 9th bit of temp to determine if there is a carry or not
    unsigned int tmp = (uint8_t)GPRS[R1] + (uint8_t)GPRS[R2_imm];

    // checks if 9th bit is 1, if it is then there is a carry
    if (tmp > 0xFF)
        set(C_FLAG);
    else
        reset(C_FLAG);

    if (((GPRS[R1] ^ (int8_t)tmp) & (GPRS[R2_imm] ^ (int8_t)tmp)) & 0x80)
        set(V_FLAG);
    else
        reset(V_FLAG);
    GPRS[R1] += GPRS[R2_imm];

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (((SREG >> N_FLAG) & 1) ^ ((SREG >> V_FLAG) & 1))
        set(S_FLAG);
    else
        reset(S_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: ADD INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void sub()
{
    int tmp = GPRS[R1];
    tmp -= GPRS[R2_imm];

    if ((GPRS[R1] ^ GPRS[R2_imm]) & ~(GPRS[R2_imm] ^ (int8_t)tmp) & 0x80)
        set(V_FLAG);
    else
        reset(V_FLAG);

    GPRS[R1] -= GPRS[R2_imm];

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (((SREG >> N_FLAG) & 1) ^ ((SREG >> V_FLAG) & 1))
        set(S_FLAG);
    else
        reset(S_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: SUB INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void mul()
{
    int tmp = GPRS[R1];
    tmp *= GPRS[R2_imm];

    GPRS[R1] *= GPRS[R2_imm];

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: MUL INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void ldi()
{
    int bit = (R2_imm >> 5) & 1;
    R2_imm |= ((1 << 7) + (1 << 6)) * bit;

    GPRS[R1] = R2_imm;

    if (!gui_mode) {
        printf("[Cycle: %i]: LDI INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void beqz()
{
    int bit = (R2_imm >> 5) & 1;
    R2_imm |= ((1 << 7) + (1 << 6)) * bit;

    if (GPRS[R1] == 0)
    {
        flush();
        PC += R2_imm - 1;
        if (!gui_mode) printf("[Cycle: %i]: BRANCH EXECUTED, PC is now %i\n", clk, PC);
    } else {
        if (!gui_mode) printf("[Cycle: %i]: R1 is not 0, DID NOT BRANCH\n", clk);
    }
}

void and()
{
    GPRS[R1] &= GPRS[R2_imm];

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: AND INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void or()
{
    GPRS[R1] |= GPRS[R2_imm];

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: OR INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void jr()
{
    PC = GPRS[R1];
    PC <<= 8;
    PC |= GPRS[R2_imm];

    if (!gui_mode) printf("[Cycle: %i]: JR EXECUTED, PC is now %i\n", clk, PC);
    flush();
}

void sal()
{
    if (R2_imm < 0)
    {
        printf("[Cycle: %i]: SAL INSTRUCTION EXECUTED, IMM = %i, IMM cannot be a negative number!\n", clk, R2_imm);
        return;
    }

    if (R2_imm >= 8)
    {
        GPRS[R1] = (int8_t)(0);
    }
    else
    {
        GPRS[R1] = (int8_t)(GPRS[R1] << R2_imm);
    }

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: SAL INSTRUCTION EXECUTED, IMM = %i, R1 = %i\n", clk, R2_imm, GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void sar()
{
    if (R2_imm < 0)
    {
        printf("[Cycle: %i]: SAR INSTRUCTION EXECUTED, IMM = %i, IMM cannot be a negative number!\n", clk, R2_imm);
        return;
    }

    if (R2_imm >= 8)
    {
        GPRS[R1] = (GPRS[R1] < 0) ? (int8_t)(-1) : (int8_t)(0);
    }
    else
    {
        GPRS[R1] = (int8_t)(GPRS[R1] >> R2_imm);
    }

    if (GPRS[R1] < 0)
        set(N_FLAG);
    else
        reset(N_FLAG);

    if (GPRS[R1] == 0)
        set(Z_FLAG);
    else
        reset(Z_FLAG);

    if (!gui_mode) {
        printf("[Cycle: %i]: SAR INSTRUCTION EXECUTED, IMM = %i, R1 = %i\n", clk, ((uint8_t)R2_imm), GPRS[R1]);
        printf("[Cycle: %i]: SREG = ", clk);
        to_bin(SREG, 8);
    }
}

void lb()
{
    int bit = (R2_imm >> 5) & 1;
    R2_imm |= ((1 << 7) + (1 << 6)) * bit;

    if (R2_imm < 0) {
        if (!gui_mode) printf("[Cycle: %i]: LB INSTRUCTION EXECUTED, ADDRESS: %i, OUT OF BOUND ERROR\n", clk, R2_imm);
        return;
    }

    GPRS[R1] = get_data(R2_imm);
    if (!gui_mode) printf("[Cycle: %i]: LB INSTRUCTION EXECUTED, R1 <- Mem[%i] = %i\n", clk, R2_imm, GPRS[R1]);
}

void sb()
{
    int bit = (R2_imm >> 5) & 1;
    R2_imm |= ((1 << 7) + (1 << 6)) * bit;

    if (R2_imm < 0) {
        if (!gui_mode) printf("[Cycle: %i]: SB INSTRUCTION EXECUTED, ADDRESS: %i, OUT OF BOUND ERROR\n", clk, R2_imm);
        return;
    }

    set_data(R2_imm, GPRS[R1]);
    if (!gui_mode) printf("[Cycle: %i]: SB INSTRUCTION EXECUTED, Mem[%i] is now %i\n", clk, R2_imm, GPRS[R1]);
}

void display_register_content(int idx)
{
    for (int i = 0; i <= idx; i++)
    {
        printf("[R%i = %i]\n", i, GPRS[i]);
    }
}
void display_memory_content(int idx)
{
    for (int i = 0; i <= idx; i++)
    {
        printf("[Mem[%i] = %i]\n", i, data_memory[i]);
    }
}
void display_instruction_content(int idx)
{
    for (int i = 0; i <= idx; i++)
    {
        printf("[Mem[%i] = %i]\n", i, instruction_memory[i]);
    }
}