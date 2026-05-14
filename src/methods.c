#include "include.h"

void add(){
    int negativeFlag=0;
    int OverflowFlag=0;
    int original8thBitR1=(R1>>7);
    int original8thBitR2=(R2_imm>>7);
    int answer=R1+R2_imm;
    /*
        Check for carry
    */
    int carryCheck=(answer>>8);
    if(carryCheck==1){
        SREG|=(1<<4);
        NegativeFlag=1;
    }
    else{
        SREG&=(~(1<<4));
        NegativeFlag=0;
    }

    int result8thBit=answer>>7;

/*
    Check for overflow
*/
if(original8thBitR1==original8thBitR2){
    if(result8thBit==original8thBitR1){
        SREG&=(~(1<<3));
        OverflowFlag=1;
    }
    else{
        SREG|=(1<<3);
        OverflowFlag=0;
    }
}

/*
    Check for negative result
*/
if(result8thBit==1){
    SREG|=(1<<2);
}
else{
    SREG&=(~(1<<2));
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
if(result==0)
{
    SREG|=(1<<0);
}
else{
    SREG&=(~(1<<0));
}

}

void sub(){
    int negativeFlag=0;
    int OverflowFlag=0;
    int original8thBitR1=(R1>>7);
    int original8thBitR2=(R2_imm>>7);
    int answer=R1-R2_imm;
    /*
        Check for carry
    */
    int carryCheck=(answer>>8);
    if(carryCheck==1){
        SREG|=(1<<4);
        NegativeFlag=1;
    }
    else{
        SREG&=(~(1<<4));
        NegativeFlag=0;
    }
    /*
        Check for overflow
    */
    int result8thBit=answer>>7;
    if(result8thBit==original8thBitR2){
        SREG&=(~(1<<3));
        OverflowFlag=1;
    }
    else{
        SREG|=(1<<3);
        OverflowFlag=0;
    }
    /*
    Check for negative result
*/
    if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
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
if(result==0)
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
    int answer=R1*R2_imm;
    /*
        Update Negative Flag
    */
   int result8thBit=answer>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
    if(result==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void ldi(){
    R1=R2_imm;
}

void beqz(){
    if(R1==0){
        flush();
        PC=PC+R2_imm;
    }
}

void and(){
    int answer=R1&R2_imm;
    /*
        Update Negative Flag
    */
   int result8thBit=answer>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
    if(result==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void or(){
    int answer=R1|R2_imm;
    /*
        Update Negative Flag
    */
   int result8thBit=answer>>7;
   if(result8thBit==1){
        SREG|=(1<<2);
    }
    else{
        SREG&=(~(1<<2));
    }
    /*
        Check for Zero Flag
    */
    if(result==0)
    {
        SREG|=(1<<0);
    }
    else{
        SREG&=(~(1<<0));
    }
}

void jr(){
    PC = (R1 << 6) | (R2_imm);
    flush();
}

void sal(){
    R1=R1<<R2_imm;
    /*
        Update Negative Flag
    */
    int result8thBit=R1>>7;
    if(result8thBit==1){
          SREG|=(1<<2);
     }
     else{
          SREG&=(~(1<<2));
     }
        /*
            Check for Zero Flag
        */
        if(result==0)
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
    int result8thBit=R1>>7;
    if(result8thBit==1){
          SREG|=(1<<2);
     }
     else{
          SREG&=(~(1<<2));
     }
        /*
            Check for Zero Flag
        */
        if(result==0)
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