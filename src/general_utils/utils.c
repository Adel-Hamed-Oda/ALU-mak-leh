#include "includes.h"
#include "utils.h"

char *instruction_map_str[NUM_OPCODES] = {
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