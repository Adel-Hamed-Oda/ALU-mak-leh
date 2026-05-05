#include "includes.h"
#include "parser.h"

EXCEPTION execute_instruction(int opcode, int r1, int r2) {
    switch (opcode) {
        case OPCODE_ADD:
            // add r1 and r2, store in r1
            break;
        case OPCODE_SUB:
            // subtract r2 from r1, store in r1
            break;
        case OPCODE_MUL:
            // multiply r1 and r2, store in r1
            break;
        case OPCODE_LDI:
            // load immediate value in r2 into r1
            break;
        case OPCODE_BEQZ:
            // branch if r1 is equal to zero, using r2 as the offset
            break;
        case OPCODE_AND:
            // and r1 and r2, store in r1
            break;
        case OPCODE_OR:
            // or r1 and r2, store in r1
            break;
        case OPCODE_JR:
            // jump to address in r1
            break;
        case OPCODE_SAL:
            // shift arithmetic left r1 by the number of bits specified in r2, store in r1
            break;
        case OPCODE_SAR:
            // shift arithmetic right r1 by the number of bits specified in r2, store in r1
            break;
        case OPCODE_LB:
            // load byte from memory address in r2 into r1
            break;
        case OPCODE_SB:
            // store byte from r1 into memory address in r2
            break;
        default:
            printf("Invalid opcode: %d\n", opcode);
    }

    return SUCCESS; // I guess
}