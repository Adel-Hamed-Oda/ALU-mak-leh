#ifndef INSTRUCTION_MANAGER_H
#define INSTRUCTION_MANAGER_H

#include <stdint.h>

/* -----------------------------------------------------------------------
 * Pipeline register: between IF and ID
 * ----------------------------------------------------------------------- */
typedef struct {
    int16_t  raw_instruction;  /* 16-bit encoded instruction            */
    uint16_t fetch_pc;         /* PC at fetch time (before increment)   */
    int      valid;            /* 1 = real instruction, 0 = bubble      */
} IF_ID_Register;

/* -----------------------------------------------------------------------
 * Pipeline register: between ID and EX
 * ----------------------------------------------------------------------- */
typedef struct {
    int      opcode;
    int      r1_index;
    int      r2_index;
    int8_t   r1_val;
    int8_t   r2_val;
    int8_t   imm_signed;    /* sign-extended 6-bit immediate             */
    uint8_t  imm_unsigned;  /* raw unsigned 6-bit immediate (shifts/addr)*/
    uint16_t fetch_pc;      /* PC at fetch time (for BEQZ calculation)  */
    int      valid;
} ID_EX_Register;

/* Global pipeline registers */
extern IF_ID_Register if_id;
extern ID_EX_Register id_ex;

/* Number of instructions loaded by the assembler */
extern int num_instructions_loaded;

/* -----------------------------------------------------------------------
 * Encode / decode
 * ----------------------------------------------------------------------- */
int16_t encode_instruction(int opcode, int field1, int field2);

/* -----------------------------------------------------------------------
 * Pipeline helpers
 * ----------------------------------------------------------------------- */
void pipeline_init(void);
void pipeline_flush_if_id(void);
void pipeline_flush_id_ex(void);

/* -----------------------------------------------------------------------
 * Instruction classification
 * ----------------------------------------------------------------------- */
int is_r_format(int opcode);
int instruction_writes_register(int opcode);  /* writes result to R1? */
int instruction_reads_r1(int opcode);         /* reads R1 as a source? */

/* -----------------------------------------------------------------------
 * Output helpers
 * ----------------------------------------------------------------------- */

/* Sign-extend a 6-bit raw value to a signed int */
int8_t sign_extend_6to8(int raw_6bit);

/* Format a 16-bit encoded instruction to human-readable string.
 * buf must be at least 32 bytes. */
void format_instruction_str(int16_t raw, char *buf);

/* Fill buf (17+ bytes) with the 16-bit binary string, e.g. "0011000001001010" */
void int16_to_binary_str(int16_t val, char *buf);

#endif /* INSTRUCTION_MANAGER_H */
