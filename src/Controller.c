#include "../include/include.h"

int8_t GPRS[GPRS_NUM] = {0};
int8_t SREG = 0;
int16_t PC = 0;

int16_t current_instruction = -1;
OPCODE opcode = MEOW;
int8_t R1 = 0;
int8_t R2_imm = 0;

int clk = 0;


void handleClkCycle() {
    OPCODE cur_opcode = opcode;
    execute();
    write_everything_to_json();

    if (cur_opcode == BEQZ || cur_opcode == JR) {
        printf("Press [ENTER] to go to the next clk cycle\n");
        getchar();
        clk++;

        return;
    }

    decode();
    fetch();

    printf("Press [ENTER] to go to the next clk cycle\n");
    getchar();
    clk++;

    return;
}

int main() {
    readProgram("program_1.txt");
    clk = 0;
    write_everything_to_json();

    while (true) {
        if (get_instr(PC) == 0xFF && opcode == MEOW && current_instruction == -1) {
            break;
        }
        handleClkCycle();
    }

    printf("[Cycle: %i]: PROGRAM FINISHED EXECUTNG\n", clk);
}

/*
gcc Controller.c data_memory.c executer.c instruction_memory.c methods.c parser.c to_json.c -o meow.exe
 */
