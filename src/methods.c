#include "../include/include.h"

extern int8_t GPRS[GPRS_NUM];
extern int8_t SREG;
extern int8_t opcode;
extern int8_t R1;
extern int8_t R2_imm;

void add(){
    GPRS[R1] += GPRS[R2_imm];
    printf("[Cycle: %i]: ADD INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void sub(){
    GPRS[R1] -= GPRS[R2_imm];
    printf("[Cycle: %i]: SUB INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void mul(){
    GPRS[R1] *= GPRS[R2_imm];
    printf("[Cycle: %i]: MUL INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void ldi(){
    GPRS[R1] = R2_imm;
    printf("[Cycle: %i]: LDI INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void beqz(){
    if(GPRS[R1] == 0){
        flush();
        PC += R2_imm - 1;
        printf("[Cycle: %i]: BRANCH EXECUTED, PC is now %i\n", clk, PC);
    } else {
        printf("[Cycle: %i]: R1 is not 0, DID NOT BRANCH\n", clk);
    }
}
void and(){
    GPRS[R1] &= GPRS[R2_imm];
    printf("[Cycle: %i]: AND INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void or(){
    GPRS[R1] |= GPRS[R2_imm];
    printf("[Cycle: %i]: OR INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void jr(){
    PC = (R1 << 6) | (R2_imm);
    printf("[Cycle: %i]: JR EXECUTED, PC is now %i\n", clk, PC);
    flush();
}
void sal(){
    GPRS[R1] <<= R2_imm; 
    printf("[Cycle: %i]: SAL INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void sar(){
    GPRS[R1] >>= R2_imm; 
    printf("[Cycle: %i]: SAR INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void lb(){
    GPRS[R1] = get_data(R2_imm);
    printf("[Cycle: %i]: LB INSTRUCTION EXECUTED, R1 = %i\n", clk, GPRS[R1]);
}
void sb(){
    set_data(R2_imm, GPRS[R1]);
    printf("[Cycle: %i]: SB INSTRUCTION EXECUTED, Mem[%i] is now %i\n", clk, R2_imm, GPRS[R1]);
}