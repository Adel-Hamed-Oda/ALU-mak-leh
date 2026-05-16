
#include "../include/include.h"

static FILE* json_file = NULL;

static FILE* open_data_json() {
    FILE* file = fopen("../gui/data.json", "w");

    if (file == NULL) {
        printf("[ERROR] Cannot open data.json!\n");
    }

    return file;
}

static void write_int8_array(int8_t arr[], int size) {
    fprintf(json_file, "[");

    for (int i = 0; i < size; i++) {
        if (i > 0) {
            fprintf(json_file, ", ");
        }

        fprintf(json_file, "%i", (int)arr[i]);
    }

    fprintf(json_file, "]");
}

void write_data_memory() {
    write_int8_array(data_memory, 64);
}

void write_instruction_memory() {
    fprintf(json_file, "[");

    for (int i = 0; i < cur; i++) {
        if (i > 0) {
            fprintf(json_file, ", ");
        }

        fprintf(json_file, "%u", (unsigned int)((uint16_t)instruction_memory[i]));
    }

    fprintf(json_file, "]");
}

void write_SREG() {
    fprintf(json_file, "%u", (unsigned int)((uint8_t)SREG));
}

void write_registers() {
    write_int8_array(GPRS, GPRS_NUM);
}

void write_clk_PC() {
    fprintf(json_file, "%i,\n    \"PC\": %i", clk, (int)PC);
}

void write_opcode() {
    fprintf(json_file, "%i", opcode);
}
void write_R1() {
    fprintf(json_file, "%i", R1);
}
void write_R2_imm() {
    fprintf(json_file, "%i", R2_imm);
}

void write_current_instruction() {
    fprintf(json_file, "%i", current_instruction);
}


void write_everything_to_json() {
    json_file = open_data_json();

    if (json_file == NULL) {
        return;
    }

    fprintf(json_file, "{\n");
    fprintf(json_file, "    \"data_memory\" : ");
    write_data_memory();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"instruction_memory\" : ");
    write_instruction_memory();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"SREG\": ");
    write_SREG();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"registers\": ");
    write_registers();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"current_instruction\": ");
    write_current_instruction();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"opcode\": ");
    write_opcode();
    fprintf(json_file, ",\n");
    
    fprintf(json_file, "    \"R1\": ");
    write_R1();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"R2_imm\": ");
    write_R2_imm();
    fprintf(json_file, ",\n");

    fprintf(json_file, "    \"clk\": ");
    write_clk_PC();
    fprintf(json_file, "\n}\n");

    fclose(json_file);
    json_file = NULL;
}
