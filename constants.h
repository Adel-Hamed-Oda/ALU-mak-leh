#define NUMBER_OF_GP_REGISTERS 64
#define NUMBER_OF_INSTRUCTION_MEMORY_WORDS 1024
#define NUMBER_OF_DATA_MEMORY_WORDS 2048

#define OPCODE_ADD  0b0000 // add
#define OPCODE_SUB  0b0001 // subtract
#define OPCODE_MUL  0b0010 // multiply
#define OPCODE_LDI  0b0011 // load immediate
#define OPCODE_BEQZ 0b0100 // branch if equal to zero
#define OPCODE_AND  0b0101 // and
#define OPCODE_OR   0b0110 // or
#define OPCODE_JR   0b0111 // jump register
#define OPCODE_SAL  0b1000 // shift arithmetic left
#define OPCODE_SAR  0b1001 // shift arithmetic right
#define OPCODE_LB   0b1010 // load byte
#define OPCODE_SB   0b1011 // store byte