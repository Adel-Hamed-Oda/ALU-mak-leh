#include "includes.h"

#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"

int main() {
    // Initialize memory
    initialize_instruction_memory();
    initialize_data_memory();

    // Example usage
    print_instruction_memory_l(0, 9);
    print_data_memory_l(0, 9);

    return 0;
}