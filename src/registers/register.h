#define C_FLAG 0b00010000
#define V_FLAG 0b00001000
#define N_FLAG 0b00000100
#define S_FLAG 0b00000010
#define Z_FLAG 0b00000001

typedef struct {
    int content : 8;
} register_8;

typedef struct {
    int content : 16;
} register_16;

void initialize_registers();

int set_register(int index, int value);
int get_register(int index, int *out_value);
int clear_register(int index);

int get_flag(int flag);
int update_status_register();

int get_pc();
void set_pc(int new_pc);
void increment_pc();

void print_registers();
void print_register(int index);