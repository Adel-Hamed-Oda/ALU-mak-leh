typedef struct {
    int value : 16;
} instruction_memory_word;

typedef struct {
    char *name; // I think we will use name for variable references
    int value : 8;
} data_memory_word;