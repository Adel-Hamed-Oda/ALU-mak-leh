#include "includes.h"
#include "assembler.h"
#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"

void reset();
char **split_by_spaces(const char *str, size_t *out_count);
void free_splits(char **splits);
EXCEPTION get_opcode_from_instruction_string(char *instruction_str, int *out_opcode);
EXCEPTION translate_register_string_to_number(char *register_str, int *out_register_number);

// turns out making a whole ass map was too complicated, so Imma stick a weird version of an array
char *instruction_map_str[] = {
    [OPCODE_ADD] = "ADD",
    [OPCODE_SUB] = "SUB",
    [OPCODE_MUL] = "MUL",
    [OPCODE_LDI] = "LDI",
    [OPCODE_BEQZ] = "BEQZ",
    [OPCODE_AND] = "AND",
    [OPCODE_OR] = "OR",
    [OPCODE_JR] = "JR",
    [OPCODE_SAL] = "SAL",
    [OPCODE_SAR] = "SAR",
    [OPCODE_LB] = "LB",
    [OPCODE_SB] = "SB"
};
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

    if (!tokens || token_count < 3) {
        free_splits(tokens);
        return ASSEMBLY_ERROR;
    }

    int opcode;
    int success = get_opcode_from_instruction_string(tokens[0], &opcode);
    if (FAIL(success)) {
        free_splits(tokens);
        return success;
    }

    char *r1 = tokens[1];
    char *r2 = tokens[2];

    int r1_index;
    int r2_index;

    success = translate_register_string_to_number(r1, &r1_index);
    if (FAIL(success)) {
        free_splits(tokens);
        return success;
    }

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
            if (!variables[free_memory_index]) {
                free_splits(tokens);
                return ASSEMBLY_ERROR; // strdup failed
            }
            variable_index = free_memory_index;
            free_memory_index++;
        }

        r2_index = variable_index;
    } else if (opcode == OPCODE_LDI || opcode == OPCODE_BEQZ || opcode == OPCODE_SAL || opcode == OPCODE_SAR) {
        char *end;
        r2_index = (int)strtol(r2, &end, 10);
        if (end == r2 || *end != '\0') {
            free_splits(tokens);
            return ASSEMBLY_ERROR; // r2 is not a valid immediate
        }
    } else {
        success = translate_register_string_to_number(r2, &r2_index);
        if (FAIL(success)) {
            free_splits(tokens);
            return success;
        }
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

    while (fgets(line, sizeof(line), file)) {
        // remove newline character
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) {
            continue; // skip empty lines
        }

        // check bounds before writing
        if (instruction_count >= NUMBER_OF_INSTRUCTION_MEMORY_WORDS) {
            fclose(file);
            return OUT_OF_BOUNDS; // too many instructions for available memory
        }

        int success = assemble_instruction(line, &out_instructions[instruction_count]);
        if (FAIL(success)) {
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

EXCEPTION translate_register_string_to_number(char *register_str, int *out_register_number) {
    if (register_str[0] != 'R') {
        return ASSEMBLY_ERROR;
    }

    char *end;
    int index = (int)strtol(register_str + 1, &end, 10);

    if (end == register_str + 1 || *end != '\0' || index < 0 || index > 63) {
        return ASSEMBLY_ERROR;
    }

    *out_register_number = index;
    return SUCCESS;
}

EXCEPTION get_opcode_from_instruction_string(char *instruction_str, int *out_opcode) {
    for (int i = 0; i < sizeof(instruction_map_str) / sizeof(instruction_map_str[0]); i++) {
        if (strcmp(instruction_str, instruction_map_str[i]) == 0) {
            *out_opcode = i & 0xF; // mask to 4 bits just in case
            return SUCCESS;
        }
    }
    return NAME_NOT_FOUND;
}

char **split_by_spaces(const char *str, size_t *out_count)
{
    if (!str) return NULL;

    char *copy = strdup(str);
    if (!copy) return NULL;

    /* First pass: count tokens. */
    size_t count = 0;
    char *p = copy;
    while (*p) {
        while (*p == ' ') p++;
        if (*p) {
            count++;
            while (*p && *p != ' ') p++;
        }
    }

    /* Allocate NULL-terminated pointer array. */
    char **result = malloc((count + 1) * sizeof(char *));
    if (!result) { free(copy); return NULL; }

    /* Second pass: copy each token into its own allocation. */
    size_t i = 0;
    p = copy;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;

        char *start = p;
        while (*p && *p != ' ') p++;

        size_t len = (size_t)(p - start);
        result[i] = malloc(len + 1);
        if (!result[i]) {
            for (size_t k = 0; k < i; k++) free(result[k]);
            free(result);
            free(copy);
            return NULL;
        }
        memcpy(result[i], start, len);
        result[i][len] = '\0';
        i++;
    }

    result[count] = NULL;
    if (out_count) *out_count = count;

    free(copy);
    return result;
}

void free_splits(char **splits)
{
    if (!splits) return;
    for (char **p = splits; *p; p++)
        free(*p);
    free(splits);
}

#pragma endregion