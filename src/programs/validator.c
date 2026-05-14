#include "includes.h"
#include "validator.h"

EXCEPTION is_valid_register(const char *token) {
    if (!token || (token[0] != 'R' && token[0] != 'r')) return REGISTER_ERROR;
    char *end;
    long n = strtol(token + 1, &end, 10);
    return (*end == '\0' && n >= 0 && n <= 63) ? SUCCESS : REGISTER_ERROR;
}

EXCEPTION is_valid_immediate(const char *token) {
    if (!token || !*token) return GENERAL_FAILURE;
    char *end;
    strtol(token, &end, 10);
    return (*end == '\0') ? SUCCESS : GENERAL_FAILURE;
}

// yeah this is stupid but who cares
EXCEPTION is_valid_string_literal(const char *token) {
    if (!token || !*token) return GENERAL_FAILURE;

    // Check if token starts with a letter or underscore
    if (!((token[0] >= 'a' && token[0] <= 'z') || 
          (token[0] >= 'A' && token[0] <= 'Z') || 
          token[0] == '_')) {
        return GENERAL_FAILURE;
    }
    
    // Check the rest of the token for alphanumeric characters or underscore
    for (int i = 1; token[i] != '\0'; i++) {
        if (!((token[i] >= 'a' && token[i] <= 'z') || 
              (token[i] >= 'A' && token[i] <= 'Z') || 
              (token[i] >= '0' && token[i] <= '9') || 
              token[i] == '_')) {
            return GENERAL_FAILURE;
        }
    }
    return SUCCESS;
}

EXCEPTION is_valid_instruction(const char *line) {
    size_t count = 0;
    char **tokens = split_by_spaces(line, &count);
    if (!tokens) return GENERAL_FAILURE;

    if (count != 3) {
        free_splits(tokens);
        return ASSEMBLY_ERROR;
    }

    // Check instruction is a known opcode
    int opcode = -1;
    for (size_t i = 0; i < NUM_OPCODES; i++) {
        if (instruction_map_str[i] && strcmp(tokens[0], instruction_map_str[i]) == 0) {
            opcode = (int)i;
            break;
        }
    }
    if (opcode == -1) {
        free_splits(tokens);
        return ASSEMBLY_ERROR;
    }

    // var1 must always be a register R0-R63
    if (FAIL(is_valid_register(tokens[1]))) {
        free_splits(tokens);
        return REGISTER_ERROR;
    }

    // var2 validation depends on instruction type
    if (opcode == OPCODE_ADD || opcode == OPCODE_SUB || opcode == OPCODE_MUL ||
        opcode == OPCODE_AND || opcode == OPCODE_OR  || opcode == OPCODE_JR) {
        // R-format: var2 must be a register
        if (FAIL(is_valid_register(tokens[2]))) {
            fprintf(stderr, "Error: '%s' expects a register as second operand, got '%s'\n",
                    tokens[0], tokens[2]);
            free_splits(tokens);
            return REGISTER_ERROR;
        }
    } else if (opcode == OPCODE_LDI || opcode == OPCODE_BEQZ) {
        // Signed 6-bit immediate: -32 to +31
        if (FAIL(is_valid_immediate(tokens[2]))) {
            fprintf(stderr, "Error: '%s' expects an integer immediate, got '%s'\n",
                    tokens[0], tokens[2]);
            free_splits(tokens);
            return ASSEMBLY_ERROR;
        }
        char *end;
        long val = strtol(tokens[2], &end, 10);
        if (val < -32 || val > 31) {
            fprintf(stderr, "Error: '%s' immediate %ld out of range [-32, 31]\n",
                    tokens[0], val);
            free_splits(tokens);
            return ASSEMBLY_ERROR;
        }
    } else if (opcode == OPCODE_SAL || opcode == OPCODE_SAR) {
        // Unsigned 6-bit shift amount: 0 to 63
        if (FAIL(is_valid_immediate(tokens[2]))) {
            fprintf(stderr, "Error: '%s' expects an integer shift amount, got '%s'\n",
                    tokens[0], tokens[2]);
            free_splits(tokens);
            return ASSEMBLY_ERROR;
        }
        char *end;
        long val = strtol(tokens[2], &end, 10);
        if (val < 0 || val > 63) {
            fprintf(stderr, "Error: '%s' shift amount %ld out of range [0, 63]\n",
                    tokens[0], val);
            free_splits(tokens);
            return ASSEMBLY_ERROR;
        }
    } else if (opcode == OPCODE_LB || opcode == OPCODE_SB) {
        // Unsigned 6-bit address: 0 to 63, or a symbolic variable name
        if (!FAIL(is_valid_immediate(tokens[2]))) {
            char *end;
            long val = strtol(tokens[2], &end, 10);
            if (val < 0 || val > 63) {
                fprintf(stderr, "Error: '%s' address %ld out of range [0, 63]\n",
                        tokens[0], val);
                free_splits(tokens);
                return ASSEMBLY_ERROR;
            }
        } else if (FAIL(is_valid_string_literal(tokens[2]))) {
            fprintf(stderr, "Error: '%s' expects an address or variable name, got '%s'\n",
                    tokens[0], tokens[2]);
            free_splits(tokens);
            return ASSEMBLY_ERROR;
        }
    }

    free_splits(tokens);
    return SUCCESS;
}
