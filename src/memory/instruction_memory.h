typedef struct {
    int value : 16;
} instruction_memory_word;

void initialize_instruction_memory();

EXCEPTION fetch_instruction(int index, int *out_value);
// DESTRUCTIVE, USE WITH CAUTION
EXCEPTION set_instruction(int index, int value);

void clear_instruction_memory();

void print_instruction_memory();
void print_instruction_memory_l(int start, int end);