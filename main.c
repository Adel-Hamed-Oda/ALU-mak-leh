#include "includes.h"

#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"
#include "src/registers/register.h"

static int clock_cycle;

int main(int argc, char *argv[]) {

}

int initialize() {
    clock_cycle = 0;

    init_instruction_memory();
    init_data_memory();
    init_registers();

    return SUCCESS;
}