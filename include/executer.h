#pragma once

#include "types.h"
#include "stdint.h"

/*
    Convert a number to binary and print it
*/

void to_bin(int16_t x, int bits);

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
    given the opcode and register and immediate
    execute the instruction
*/
void execute();



/*  
    set current instruction to be -1
*/
void flush();
