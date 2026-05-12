typedef struct {
    int content : 8;
} register_8;

typedef struct {
    int content : 16;
} register_16;

void initialize_registers(void);

EXCEPTION set_register(int index, int value);
EXCEPTION get_register(int index, int *out_value);
EXCEPTION clear_register(int index);

/* --- PC --- */
int  get_pc(void);
void set_pc(int new_pc);
void increment_pc(void);

/* --- SREG --- */
int  get_flag(STATUS_FLAG flag);
void set_sreg_flag(STATUS_FLAG flag, int value);
int  get_sreg_full(void);
void set_sreg_full(int val);

/* --- Debug / dump --- */
void print_registers(void);
void print_register(int index);
void dump_all_registers(void);
