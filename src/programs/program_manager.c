#include "includes.h"
#include "program_manager.h"

static program* programs;



void print_program_info(int index) {
    printf("Program Name: %s\n", programs[index].name);
    printf("Original File Name: %s\n", programs[index].original_file_name);
    printf("Instruction Count: %d\n", programs[index].instruction_count);

    for (int i = 0; i < programs[index].instruction_count; i++) {
        printf("Instruction %d: %d\n", i, programs[index].instructions[i]);
    }
}