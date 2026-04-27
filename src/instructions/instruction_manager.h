#define OPCODE_ADD 0b0000 // add
#define OPCODE_SUB 0b0001 // subtract
#define OPCODE_MUL 0b0010 // multiply
#define OPCODE_LDI 0b0011 // load immediate
#define OPCODE_BEZ 0b0100 // branch if equal to zero
#define OPCODE_AND 0b0101 // and
#define OPCODE_OR  0b0110 // or
#define OPCODE_JR  0b0111 // jump register
#define OPCODE_SAL 0b1000 // shift arithmetic left
#define OPCODE_SAR 0b1001 // shift arithmetic right
#define OPCODE_LB  0b1010 // load byte
#define OPCODE_SB  0b1011 // store byte

typedef struct {
    int opcode : 4;
    int r1 : 6;
    int r2 : 6;
} instruction;

// since immediate is the same structure as normal
// I will just use the same struct for both

// BE CAREFUL, THIS SHIT WRAPS, NO ERROR OR CRASHING FA
// SOMETHING MIGHT LOGICALLY GO WRONG BECAUSE OF IT