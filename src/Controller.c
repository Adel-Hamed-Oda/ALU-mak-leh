#include "../include/include.h"

int8_t GPRS[GPRS_NUM] = {0};
int8_t SREG = 0;
int16_t PC = 0;

int16_t current_instruction = -1;
int8_t opcode = MEOW;
int8_t R1 = 0;
int8_t R2_imm = 0;

int clk = 0;

int main() {
    readProgram("program.txt");
    clk = 0;

    while (get_instr(PC) != 0xFF) {
        execute();

        if (opcode == BEQZ || opcode == JR) {
            clk++;
        }

        decode();
        fetch();

        if (opcode != BEQZ && opcode != JR) {
            clk++;
        }
    }
}

/*
gcc Controller.c data_memory.c executer.c instruction_memory.c methods.c parser.c -o meow.exe
 */
