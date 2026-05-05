#define NUMBER_OF_GP_REGISTERS 64
#define NUMBER_OF_INSTRUCTION_MEMORY_WORDS 1024
#define NUMBER_OF_DATA_MEMORY_WORDS 2048

typedef enum {
    OPCODE_ADD  = 0b0000, // add
    OPCODE_SUB  = 0b0001, // subtract
    OPCODE_MUL  = 0b0010, // multiply
    OPCODE_LDI  = 0b0011, // load immediate
    OPCODE_BEQZ = 0b0100, // branch if equal to zero
    OPCODE_AND  = 0b0101, // and
    OPCODE_OR   = 0b0110, // or
    OPCODE_JR   = 0b0111, // jump register
    OPCODE_SAL  = 0b1000, // shift arithmetic left
    OPCODE_SAR  = 0b1001, // shift arithmetic right
    OPCODE_LB   = 0b1010, // load byte
    OPCODE_SB   = 0b1011  // store byte
} OPCODE;

typedef enum {
    C_FLAG = 0b00010000,
    V_FLAG = 0b00001000,
    N_FLAG = 0b00000100,
    S_FLAG = 0b00000010,
    Z_FLAG = 0b00000001
} STATUS_FLAG;