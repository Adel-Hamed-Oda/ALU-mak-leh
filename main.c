#include "includes.h"

#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"
#include "src/registers/register.h"
#include "src/instructions/parser.h"
#include "src/instructions/instruction_manager.h"
#include "src/programs/assembler.h"

static int clock_cycle;

int main(int argc, char *argv[]) {
    load_program_from_file("programs/test0.txt");

    print_instruction_memory_l(0, 20);
    print_data_memory_l(0, 10);
}

int initialize() {
    clock_cycle = 0;

    initialize_instruction_memory();
    initialize_data_memory();
    initialize_registers();

    return SUCCESS;
}