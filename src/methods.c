#include "../include/include.h"

void add(){
    int NegativeFlag=0;
    int OverflowFlag=0;
    int original8thBitR1=(GPRS[R1]>>7);
    int original8thBitR2=(GPRS[R2_imm]>>7);
    int answer=GPRS[R1]+GPRS[R2_imm];
    GPRS[R1] = (int8_t) answer;
    /*
        Check for carry
    */
    int carryCheck=(answer&0x100)>>8;
    if(carryCheck==1){
        SREG|=(1<<4);
    }
    else{
        SREG&=(~(1<<4));
    }

    int result8thBit=answer>>7;

/*
    Check for overflow
*/
if (original8thBitR1==original8thBitR2) {
    if (result8thBit==original8thBitR1) {
        SREG&=~(1<<3);
        OverflowFlag=0;
    } else {
        SREG|=(1<<3);
        OverflowFlag=1;
    }
} else {
    SREG&=~(1<<3);
    OverflowFlag=0;
}

/*
    Check for negative result
*/
if(result8thBit==1){
    SREG|=(1<<2);
    NegativeFlag=1;
}
else{
    SREG&=(~(1<<2));
    NegativeFlag=0;
}
/*
    Check for Sign Bit
*/
if(OverflowFlag^NegativeFlag==1){
    SREG|=(1<<1);
}
else{
    SREG&=(~(1<<1));
}

/*
    Check for Zero Flag
*/
if(GPRS[R1]==0)
{
    SREG|=(1<<0);
}
else{
    SREG&=(~(1<<0));
}

}

void sub(){
    int NegativeFlag=0;
    int OverflowFlag=0;
    int original8thBitR1=(GPRS[R1]>>7);
    int original8thBitR2=(GPRS[R2_imm]>>7);
    int answer=GPRS[R1]-GPRS[R2_imm];
    GPRS[R1] = (int8_t) answer;
    /*
        Check for overflow
    */
    int result8thBit=GPRS[R1]>>7;
if (original8thBitR1!=original8thBitR2) {
    if (result8thBit!=original8thBitR2) {
        SREG&=~(1<<3);
        OverflowFlag=0;
    } else {
        SREG|=(1<<3);
        OverflowFlag=1;
    }
} else {
    SREG&=~(1<<3);
    OverflowFlag=0;
}
    /*
    Check for negative result
*/
    if(result8thBit==1){
        SREG|=(1<<2);
        NegativeFlag=1;
    }
    else{
        SREG&=(~(1<<2));
        NegativeFlag=0;
    }
/*
     Check for Sign Bit
*/
if(OverflowFlag^NegativeFlag==1){
    SREG|=(1<<1);
}
else{
    SREG&=(~(1<<1));
}

/*
    Check for Zero Flag
*/
if(GPRS[R1]==0)
{
    SREG|=(1<<0);
}
else{
    SREG&=(~(1<<0));
}
/*
    Need to store result in a REGISTER!!
*/
}

void mul(){
    int answer=GPRS[R1]*GPRS[R2_imm];
    GPRS[R1] = (int8_t) answer;
    /*
        Update Negative Flag
    */
   int result8thBit=GPRS[R1]>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
   int zerocheck=GPRS[R1];
    if(zerocheck==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void ldi(){
    GPRS[R1]=R2_imm;
}

void beqz(){
    if(GPRS[R1]==0){
        flush();
        PC=PC+R2_imm-1;
    }
}

void and(){
    int answer=GPRS[R1]&GPRS[R2_imm];
    GPRS[R1] = (int8_t) answer;
    /*
        Update Negative Flag
    */
   int result8thBit=GPRS[R1]>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
   int zerocheck=GPRS[R1];
    if(zerocheck==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void or(){
    int answer=GPRS[R1]|GPRS[R2_imm];
    GPRS[R1] = (int8_t) answer;
    /*
        Update Negative Flag
    */
   int result8thBit=GPRS[R1]>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
    if(GPRS[R1]==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void jr(){
    int AND = 0b111111;
    PC = ((R1 & AND) << 6) | (R2_imm & AND);
    flush();
}

void sal(){
    GPRS[R1]=GPRS[R1]<<R2_imm;
    /*
        Update Negative Flag
    */
    int result8thBit=GPRS[R1]>>7;
    if(result8thBit==1){
          SREG|=(1<<2);
     }
     else{
          SREG&=(~(1<<2));
     }
        /*
            Check for Zero Flag
        */
        if(GPRS[R1]==0)
        {
            SREG|=(1<<0);
        }
        else{
            SREG&=(~(1<<0));
        }
}

void sar(){
    GPRS[R1] >>= R2_imm;
    /*
        Update Negative Flag
    */
    int result8thBit=GPRS[R1]>>7;
    if(result8thBit==1){
          SREG|=(1<<2);
     }
     else{
          SREG&=(~(1<<2));
     }
        /*
            Check for Zero Flag
        */
        if(GPRS[R1]==0)
        {
            SREG|=(1<<0);
        }
        else{
            SREG&=(~(1<<0));
        }
}

void lb(){
    GPRS[R1] = get_data(R2_imm);
}

void sb(){
    set_data(R2_imm, GPRS[R1]);
}
