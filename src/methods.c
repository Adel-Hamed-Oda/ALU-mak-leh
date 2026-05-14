#include "../include/include.h"

extern int8_t GPRS[GPRS_NUM];
extern int8_t SREG;
extern int8_t opcode;
extern int8_t R1;
extern int8_t R2_imm;

void add(){
    GPRS[R1] += GPRS[R2_imm];
}
void sub(){
    GPRS[R1] -= GPRS[R2_imm];
}
void mul(){
    GPRS[R1] *= GPRS[R2_imm];
}
void ldi(){
    GPRS[R1] = R2_imm;
}
void beqz(){
    if(GPRS[R1] == 0){
        flush();
        PC += R2_imm;
    }
}
void and(){
    GPRS[R1] &= GPRS[R2_imm];
}
void or(){
    GPRS[R1] |= GPRS[R2_imm];
}
void jr(){
    PC = (R1 << 6) | (R2_imm);
    flush();
}
void sal(){
    GPRS[R1] <<= R2_imm; 
}
void sar(){
    GPRS[R1] >>= R2_imm; 
}
void lb(){
    GPRS[R1] = get_data(R2_imm);
}
void sb(){
    set_data(R2_imm, GPRS[R1]);
}