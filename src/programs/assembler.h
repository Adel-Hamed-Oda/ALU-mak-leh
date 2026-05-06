#include "validator.h"

EXCEPTION load_program_from_file(char *file_name);
EXCEPTION assemble_program_from_file(char *file_name, int *out_instructions, int *out_instruction_count);