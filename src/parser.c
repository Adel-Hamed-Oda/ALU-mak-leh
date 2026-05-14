#include "../include/include.h"

int8_t getOpcode(string str) {
    if (strcmp(str, "ADD") == 0) return ADD;
    if (strcmp(str, "SUB") == 0) return SUB;
    if (strcmp(str, "MUL") == 0) return MUL;
    if (strcmp(str, "LDI") == 0) return LDI;
    if (strcmp(str, "BEQZ") == 0) return BEQZ;
    if (strcmp(str, "AND") == 0) return AND;
    if (strcmp(str, "OR") == 0) return OR;
    if (strcmp(str, "JR") == 0) return JR;
    if (strcmp(str, "SAL") == 0) return SAL;
    if (strcmp(str, "SAR") == 0) return SAR;
    if (strcmp(str, "LB") == 0) return LB;
    return SB;
}

int8_t getR1(string str) {
    return atoi(str + 1);
}

int8_t getR2_imm(string str) {
    if (str[0] == 'R') {
        return getR1(str);
    } 
    return atoi(str);
}

void readProgram(string filename) {
    char dir[64];
    sprintf(dir, "../programs/%s", filename);

    FILE* f = fopen(dir, "r");
    
    if (f == NULL) {
        printf("[ERROR] Cannot open file!\n");
        return;
    }

    char line[1024];

    while (fgets(line, sizeof(line), f) != NULL) {
        char _opcode[20], _R1[20], _R2_imm[20];
        sscanf(line, "%s %s %s", _opcode, _R1, _R2_imm);

        int8_t opcode = getOpcode(_opcode);
        int8_t R1 = getR1(_R1);
        int8_t R2_imm = getR2_imm(_R2_imm);

        int16_t instruction = (opcode << 12) + (R1 << 6) + R2_imm;

        push_back_instr(instruction);
    }

    push_back_instr(0xFF);
    fclose(f);
}