#include "includes.h"
#include "instruction_manager.h"

/* Global pipeline registers */
IF_ID_Register if_id;
ID_EX_Register id_ex;

/* -----------------------------------------------------------------------
 * Encode / decode
 * ----------------------------------------------------------------------- */

int16_t encode_instruction(int opcode, int field1, int field2) {
    int16_t instr = 0;
    instr |= (int16_t)((opcode & 0xF)  << 12);
    instr |= (int16_t)((field1 & 0x3F) << 6);
    instr |= (int16_t)( field2 & 0x3F);
    return instr;
}

/* -----------------------------------------------------------------------
 * Pipeline helpers
 * ----------------------------------------------------------------------- */

void pipeline_init(void) {
    if_id.raw_instruction = 0;
    if_id.fetch_pc        = 0;
    if_id.valid           = 0;

    id_ex.opcode       = 0;
    id_ex.r1_index     = 0;
    id_ex.r2_index     = 0;
    id_ex.r1_val       = 0;
    id_ex.r2_val       = 0;
    id_ex.imm_signed   = 0;
    id_ex.imm_unsigned = 0;
    id_ex.fetch_pc     = 0;
    id_ex.valid        = 0;
}

void pipeline_flush_if_id(void) {
    if_id.valid = 0;
    if_id.raw_instruction = 0;
    if_id.fetch_pc = 0;
}

void pipeline_flush_id_ex(void) {
    id_ex.valid = 0;
}

/* -----------------------------------------------------------------------
 * Instruction classification
 * ----------------------------------------------------------------------- */

int is_r_format(int opcode) {
    switch (opcode) {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_JR:
            return 1;
        default:
            return 0;
    }
}

int instruction_writes_register(int opcode) {
    /* All instructions except BEQZ, JR, SB write to R1 */
    switch (opcode) {
        case OPCODE_BEQZ:
        case OPCODE_JR:
        case OPCODE_SB:
            return 0;
        default:
            return 1;
    }
}

int instruction_reads_r1(int opcode) {
    /* Instructions that read R1 as a source value */
    switch (opcode) {
        case OPCODE_LDI:
        case OPCODE_LB:
            return 0;  /* LDI and LB only write R1, never read it */
        default:
            return 1;
    }
}

/* -----------------------------------------------------------------------
 * Output helpers
 * ----------------------------------------------------------------------- */

int8_t sign_extend_6to8(int raw_6bit) {
    raw_6bit &= 0x3F;
    return (raw_6bit & 0x20) ? (int8_t)(raw_6bit - 64) : (int8_t)raw_6bit;
}

void int16_to_binary_str(int16_t val, char *buf) {
    for (int i = 15; i >= 0; i--)
        buf[15 - i] = ((val >> i) & 1) ? '1' : '0';
    buf[16] = '\0';
}

void format_instruction_str(int16_t raw, char *buf) {
    int opcode  = (raw >> 12) & 0xF;
    int r1      = (raw >> 6)  & 0x3F;
    int r2_raw  = raw & 0x3F;
    int8_t imm  = sign_extend_6to8(r2_raw);

    if (opcode >= NUM_OPCODES || !instruction_map_str[opcode]) {
        sprintf(buf, "UNKNOWN");
        return;
    }

    const char *mn = instruction_map_str[opcode];

    switch (opcode) {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_JR:
            sprintf(buf, "%s R%d R%d", mn, r1, r2_raw);
            break;
        case OPCODE_LDI:
        case OPCODE_BEQZ:
            sprintf(buf, "%s R%d %d", mn, r1, (int)imm);
            break;
        case OPCODE_SAL:
        case OPCODE_SAR:
        case OPCODE_LB:
        case OPCODE_SB:
            /* shift amount and address are always unsigned */
            sprintf(buf, "%s R%d %d", mn, r1, r2_raw);
            break;
        default:
            sprintf(buf, "UNKNOWN");
    }
}
