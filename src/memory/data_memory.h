typedef struct {
    char *name; // I think we will use name for variable references
    int value : 8;
} data_memory_word;

void initialize_data_memory();

void print_data_memory();
void print_data_memory_l(int start, int end);