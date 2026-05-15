#pragma once

#define INSTRUCTION_MEMORY_SIZE 1024
#define DATA_MEMORY_SIZE 2048
#define GPRS_NUM 64

typedef enum {
    ADD,                            // 0000
    SUB,                            // 0001
    MUL,                            // 0010
    LDI,                            // 0011
    BEQZ,                           // 0100
    AND,                            // 0101
    OR,                             // 0110
    JR,                             // 0111
    SAL,                            // 1000
    SAR,                            // 1001
    LB,                             // 1010
    SB,                             // 1011
    MEOW                            // 1110  
} OPCODE;

typedef char* string;