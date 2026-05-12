#include "includes.h"
#include "assembler.h"
#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"

/* Defined here; declared extern in assembler.h and instruction_manager.h */
int num_instructions_loaded = 0;

static void reset(void);
static void get_opcode_from_instruction_string(char *instruction_str, int *out_opcode);
static void translate_register_string_to_number(char *register_str, int *out_register_number);
static int  token_is_integer(const char *token);

/* Variable-name → data-memory-address mapping for symbolic LB/SB operands */
static char *variables[NUMBER_OF_DATA_MEMORY_WORDS];
static int   free_memory_index;

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

EXCEPTION load_program_from_file(char *file_name) {
    reset();

    int instruction_count = 0;
    int instructions[NUMBER_OF_INSTRUCTION_MEMORY_WORDS];

    int success = assemble_program_from_file(file_name, instructions, &instruction_count);
    if (FAIL(success))
        return success;

    for (int i = 0; i < instruction_count; i++) {
        int ok = set_instruction(i, instructions[i]);
        if (FAIL(ok))
            return ok;
    }

    num_instructions_loaded = instruction_count;
    return SUCCESS;
}

static EXCEPTION assemble_instruction(char *assembly_line, int *out_instruction) {
    size_t token_count;
    char **tokens = split_by_spaces(assembly_line, &token_count);

    int opcode;
    get_opcode_from_instruction_string(tokens[0], &opcode);

    char *t1 = tokens[1];
    char *t2 = tokens[2];

    int r1_index;
    int r2_index;

    translate_register_string_to_number(t1, &r1_index);

    if (opcode == OPCODE_LB || opcode == OPCODE_SB) {
        if (token_is_integer(t2)) {
            /* Numeric address like "SB R1 0" or "LB R2 5" */
            char *end;
            r2_index = (int)strtol(t2, &end, 10);
        } else {
            /* Symbolic variable name like "SB R1 result" */
            int variable_index = -1;
            for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
                if (variables[i] && strcmp(variables[i], t2) == 0) {
                    variable_index = i;
                    break;
                }
            }
            if (variable_index == -1) {
                if (free_memory_index >= NUMBER_OF_DATA_MEMORY_WORDS) {
                    free_splits(tokens);
                    return OUT_OF_BOUNDS;
                }
                variables[free_memory_index] = strdup(t2);
                variable_index = free_memory_index++;
            }
            r2_index = variable_index;
        }
    } else if (opcode == OPCODE_LDI  ||
               opcode == OPCODE_BEQZ ||
               opcode == OPCODE_SAL  ||
               opcode == OPCODE_SAR) {
        char *end;
        r2_index = (int)strtol(t2, &end, 10);
    } else {
        translate_register_string_to_number(t2, &r2_index);
    }

    *out_instruction = ((opcode & 0xF) << 12)
                     | ((r1_index & 0x3F) << 6)
                     | (r2_index & 0x3F);

    free_splits(tokens);
    return SUCCESS;
}

EXCEPTION assemble_program_from_file(char *file_name, int *out_instructions, int *out_instruction_count) {
    FILE *file = fopen(file_name, "r");
    if (!file)
        return ASSEMBLY_ERROR;

    char line[256];
    int  instruction_count = 0;
    int  line_number       = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0)
            continue;

        if (FAIL(is_valid_instruction(line))) {
            fprintf(stderr, "Error: Invalid instruction at %s:%d -> '%s'\n",
                    file_name, line_number, line);
            fclose(file);
            return ASSEMBLY_ERROR;
        }

        if (instruction_count >= NUMBER_OF_INSTRUCTION_MEMORY_WORDS) {
            fprintf(stderr, "Error: Exceeded instruction memory at %s:%d\n",
                    file_name, line_number);
            fclose(file);
            return OUT_OF_BOUNDS;
        }

        if (FAIL(assemble_instruction(line, &out_instructions[instruction_count]))) {
            fprintf(stderr, "Error: Failed to assemble at %s:%d -> '%s'\n",
                    file_name, line_number, line);
            fclose(file);
            return ASSEMBLY_ERROR;
        }

        instruction_count++;
    }

    fclose(file);
    *out_instruction_count = instruction_count;
    return SUCCESS;
}

/* -----------------------------------------------------------------------
 * Internal helpers
 * ----------------------------------------------------------------------- */

static void reset(void) {
    clear_instruction_memory();
    clear_data_memory();
    for (int i = 0; i < NUMBER_OF_DATA_MEMORY_WORDS; i++) {
        free(variables[i]);
        variables[i] = NULL;
    }
    free_memory_index = 0;
}

static void translate_register_string_to_number(char *register_str, int *out_register_number) {
    char *end;
    *out_register_number = (int)strtol(register_str + 1, &end, 10);
}

static void get_opcode_from_instruction_string(char *instruction_str, int *out_opcode) {
    for (int i = 0; i < NUM_OPCODES; i++) {
        if (instruction_map_str[i] && strcmp(instruction_str, instruction_map_str[i]) == 0) {
            *out_opcode = i & 0xF;
            return;
        }
    }
    *out_opcode = -1;
}

static int token_is_integer(const char *token) {
    if (!token || !*token) return 0;
    const char *p = token;
    if (*p == '-' || *p == '+') p++;
    if (!*p) return 0;
    while (*p) {
        if (*p < '0' || *p > '9') return 0;
        p++;
    }
    return 1;
}
