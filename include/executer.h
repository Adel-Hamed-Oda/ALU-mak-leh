#include "types.h"
#include "stdint.h"

/*
    get the current instruction the PC is pointing at and store it into current_instruction
*/
void fetch();

/*
    get the opcode of the current_instruction
    find the values of the Registers/immediate
    call either execute_R or execute_I
    increment the PC
*/
void decode();

/*
    given the opcode and following registers
    execute the instruction
*/
void execute_R(int opcode, int R1, int R2);

/*
    given the opcode and register and immediate
    execute the instruction
*/
void execute_I(int opcode, int R1, int imm);



/*  
    set current instruction to be -1
*/
void flush();