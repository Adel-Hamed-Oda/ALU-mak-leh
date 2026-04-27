typedef struct {
    int value : 16;
} instruction_memory_word;

void initialize_instruction_memory();

int fetch_instruction(int index);
// DESTRUCTIVE, USE WITH CAUTION
int set_instruction(int index, int value);

void print_instruction_memory();
void print_instruction_memory_l(int start, int end);