typedef struct {
    int content : 8;
} register_8;

typedef struct {
    int content : 16;
} register_16;

void initialize_registers();

EXCEPTION set_register(int index, int value);
EXCEPTION get_register(int index, int *out_value);
EXCEPTION clear_register(int index);

int get_flag(STATUS_FLAG flag);
void update_status_register();

int get_pc();
void set_pc(int new_pc);
void increment_pc();

void print_registers();
void print_register(int index);