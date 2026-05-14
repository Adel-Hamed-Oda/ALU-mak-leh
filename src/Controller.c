#include "../include/include.h"

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

// gcc Controller.c data_memory.c executer.c instruction_memory.c methods.c parser.c -o meow.exe