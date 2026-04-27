typedef struct {
    char *name;
    char *original_file_name;
    int* instructions;
    int instruction_count;
} program;

int load_program_from_file(char *file_name);

int get_program_instructions_n(char *name, int out_instructions[]);
int get_program_instructions_i(int index, int out_instructions[]);
int get_program_instruction_count_n(char *name, int *out_instruction_count);
int get_program_instruction_count_i(int index, int *out_instruction_count);