#include "validator.h"

/* Number of instructions successfully loaded by the last call to
 * load_program_from_file(). Defined in assembler.c, declared here for the
 * simulation loop in main.c. */
extern int num_instructions_loaded;

EXCEPTION load_program_from_file(char *file_name);
EXCEPTION assemble_program_from_file(char *file_name, int *out_instructions, int *out_instruction_count);
