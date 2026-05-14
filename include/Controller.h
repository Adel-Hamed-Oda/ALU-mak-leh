#include "registers.h"

int16_t current_instruction = -1;
extern int16_t PC;

int8_t opcode;
int8_t R1;
int8_t R2_imm;

int clk;