typedef struct {
    int value : 8;
} data_memory_word;

void    initialize_data_memory(void);
void    clear_data_memory(void);
EXCEPTION get_data(int index, int *out_value);
EXCEPTION set_data(int index, int value);
void    print_data_memory(void);
void    print_data_memory_l(int start, int end);
