typedef struct {
    int opcode : 4;
    int r1 : 6; // BE CAREFUL, THIS SHIT WRAPS, NO ERROR OR CRASHING FA
    int r2 : 6; // SOMETHING MIGHT LOGICALLY GO WRONG BECAUSE OF IT
} instruction;

// since immediate is the same structure as normal
// I will just use the same struct for both

// should take in 16 bits
instruction decode_instruction(int instruction_word);

// should return 16 bits
int encode_instruction(instruction instr);