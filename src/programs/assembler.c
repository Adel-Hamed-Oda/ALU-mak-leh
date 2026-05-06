#include "includes.h"
#include "assembler.h"
#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"

void reset();
void get_opcode_from_instruction_string(char *instruction_str, int *out_opcode);
void translate_register_string_to_number(char *register_str, int *out_register_number);

// akeed no more variables than number of available data memory words
char *variables[NUMBER_OF_DATA_MEMORY_WORDS];

static int free_memory_index;

EXCEPTION load_program_from_file(char *file_name) {
    reset();

    int instruction_count = 0;
    int instructions[NUMBER_OF_INSTRUCTION_MEMORY_WORDS];

    int success = assemble_program_from_file(file_name, instructions, &instruction_count);
    if (FAIL(success)) {
        return success;
    }

    for (int i = 0; i < instruction_count; i++) {
        int success = set_instruction(i, instructions[i]);
        if (FAIL(success)) {
            return success;
        }
    }

    return SUCCESS;
}

EXCEPTION assemble_instruction(char *assembly_line, int *out_instruction) {
    size_t token_count;
    char **tokens = split_by_spaces(assembly_line, &token_count);

    int opcode;
    get_opcode_from_instruction_string(tokens[0], &opcode);

    char *r1 = tokens[1];
    char *r2 = tokens[2];

    int r1_index;
    int r2_index;

    translate_register_string_to_number(r1, &r1_index);

    if (opcode == OPCODE_LB || opcode == OPCODE_SB) {
        // for these 2, r2 is actually a variable name, so we need to check if it exists in the variables array
        int variable_index = -1;
        for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
            if (variables[i] && strcmp(variables[i], r2) == 0) {
                variable_index = i;
                break;
            }
        }

        // if the variable doesn't exist, we need to add it to the variables array and increment the free memory index
        if (variable_index == -1) {
            if (free_memory_index >= NUMBER_OF_DATA_MEMORY_WORDS) {
                free_splits(tokens);
                return OUT_OF_BOUNDS; // no more memory available for variables
            }
            variables[free_memory_index] = strdup(r2);
            variable_index = free_memory_index;
            free_memory_index++;
        }

        r2_index = variable_index;
    } else if (opcode == OPCODE_LDI || opcode == OPCODE_BEQZ || opcode == OPCODE_SAL || opcode == OPCODE_SAR) {
        char *end;
        r2_index = (int)strtol(r2, &end, 10);
    } else {
        translate_register_string_to_number(r2, &r2_index);
    }

    // mask each field to prevent bit corruption
    *out_instruction = ((opcode & 0xF) << 12) | ((r1_index & 0x3F) << 6) | (r2_index & 0x3F);

    free_splits(tokens);
    return SUCCESS;
}

EXCEPTION assemble_program_from_file(char *file_name, int *out_instructions, int *out_instruction_count) {
    FILE *file = fopen(file_name, "r");
    if (!file) {
        return ASSEMBLY_ERROR;
    }

    char line[256];
    int instruction_count = 0;
    int line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        // remove newline character
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) {
            continue; // skip empty lines
        }

        if (FAIL(is_valid_instruction(line))) {
            fprintf(stderr, "Error: Invalid instruction at %s:%d -> '%s'\n", file_name, line_number, line);
            fclose(file);
            return ASSEMBLY_ERROR;
        }

        // check bounds before writing
        if (instruction_count >= NUMBER_OF_INSTRUCTION_MEMORY_WORDS) {
            fprintf(stderr, "Error: Exceeded instruction memory words at %s:%d\n", file_name, line_number);
            fclose(file);
            return OUT_OF_BOUNDS; // too many instructions for available memory
        }

        int success = assemble_instruction(line, &out_instructions[instruction_count]);
        if (FAIL(success)) {
            fprintf(stderr, "Error: Failed to assemble instruction at %s:%d -> '%s'\n", file_name, line_number, line);
            fclose(file);
            return success;
        }

        instruction_count++;
    }

    fclose(file);
    *out_instruction_count = instruction_count;
    return SUCCESS;
}

void reset() {
    clear_instruction_memory();
    clear_data_memory();

    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        free(variables[i]); // free strdup'd strings before nulling
        variables[i] = NULL;
    }

    free_memory_index = 0;
}

#pragma region Helper Functions

void translate_register_string_to_number(char *register_str, int *out_register_number) {
    char *end;
    *out_register_number = (int)strtol(register_str + 1, &end, 10);
}

void get_opcode_from_instruction_string(char *instruction_str, int *out_opcode) {
    for (int i = 0; i < sizeof(instruction_map_str) / sizeof(instruction_map_str[0]); i++) {
        if (strcmp(instruction_str, instruction_map_str[i]) == 0) {
            *out_opcode = i & 0xF; // mask to 4 bits just in case
            return;
        }
    }
}

#pragma endregion