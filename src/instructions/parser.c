#include "includes.h"
#include "parser.h"
#include "instruction_manager.h"
#include "src/registers/register.h"
#include "src/memory/data_memory.h"

/* -----------------------------------------------------------------------
 * Internal helpers
 * ----------------------------------------------------------------------- */

static void update_nz_flags(int8_t result) {
    int N = (result < 0) ? 1 : 0;
    int Z = (result == 0) ? 1 : 0;
    set_sreg_flag(N_FLAG, N);
    set_sreg_flag(Z_FLAG, Z);
}

static void print_sreg_flags(int sreg) {
    int C = (sreg & (int)C_FLAG) ? 1 : 0;
    int V = (sreg & (int)V_FLAG) ? 1 : 0;
    int N = (sreg & (int)N_FLAG) ? 1 : 0;
    int S = (sreg & (int)S_FLAG) ? 1 : 0;
    int Z = (sreg & (int)Z_FLAG) ? 1 : 0;
    printf("C=%d, V=%d, N=%d, S=%d, Z=%d", C, V, N, S, Z);
}

static void notify_reg_change(int idx, int old_val, int new_val) {
    if (old_val != new_val)
        printf("  [EX] Register R%d updated: %d -> %d\n", idx, old_val, new_val);
}

static void notify_sreg_change(int old_sreg, int new_sreg) {
    if (old_sreg != new_sreg) {
        printf("  [EX] SREG updated: 0x%02X -> 0x%02X  (", old_sreg, new_sreg);
        print_sreg_flags(new_sreg);
        printf(")\n");
    }
}

static void notify_mem_change(int addr, int old_val, int new_val) {
    if (old_val != new_val)
        printf("  [EX] Data Memory[%d] updated: %d -> %d\n", addr, old_val, new_val);
}

/* -----------------------------------------------------------------------
 * execute_stage — main dispatch
 * ----------------------------------------------------------------------- */

void execute_stage(ID_EX_Register d) {
    printf("  Stage EX:\n");

    if (!d.valid) {
        printf("    Instruction: No instruction (bubble)\n");
        return;
    }

    char instr_str[32];
    /* Reconstruct raw encoding just for display */
    int16_t raw_display = encode_instruction(d.opcode, d.r1_index,
                                             is_r_format(d.opcode)
                                             ? d.r2_index
                                             : (int)d.imm_unsigned);
    format_instruction_str(raw_display, instr_str);
    printf("    Instruction: %s\n", instr_str);

    int old_sreg = get_sreg_full();
    int old_r1   = 0;
    get_register(d.r1_index, &old_r1);

    switch (d.opcode) {

    /* ---- ADD ---- */
    case OPCODE_ADD: {
        int8_t a = d.r1_val, b = d.r2_val;
        int ua = (int)(uint8_t)a, ub = (int)(uint8_t)b;
        int ur = ua + ub;
        int8_t result = (int8_t)(ur & 0xFF);

        int C = (ur >> 8) & 1;
        int N = (result < 0) ? 1 : 0;
        int Z = (result == 0) ? 1 : 0;
        int V = ((a >= 0 && b >= 0 && result < 0) ||
                 (a < 0  && b < 0  && result >= 0)) ? 1 : 0;
        int S = N ^ V;

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)a, d.r2_index, (int)b);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        set_sreg_flag(C_FLAG, C);
        set_sreg_flag(V_FLAG, V);
        set_sreg_flag(N_FLAG, N);
        set_sreg_flag(S_FLAG, S);
        set_sreg_flag(Z_FLAG, Z);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- SUB ---- */
    case OPCODE_SUB: {
        int8_t a = d.r1_val, b = d.r2_val;
        int8_t result = (int8_t)((int)a - (int)b);

        int N = (result < 0) ? 1 : 0;
        int Z = (result == 0) ? 1 : 0;
        int V = ((a >= 0 && b < 0 && result < 0) ||
                 (a < 0  && b >= 0 && result >= 0)) ? 1 : 0;
        int S = N ^ V;

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)a, d.r2_index, (int)b);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        set_sreg_flag(V_FLAG, V);
        set_sreg_flag(N_FLAG, N);
        set_sreg_flag(S_FLAG, S);
        set_sreg_flag(Z_FLAG, Z);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- MUL ---- */
    case OPCODE_MUL: {
        int8_t a = d.r1_val, b = d.r2_val;
        int product = (int)a * (int)b;
        int8_t result = (int8_t)(product & 0xFF);

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)a, d.r2_index, (int)b);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        update_nz_flags(result);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- LDI ---- */
    case OPCODE_LDI: {
        int8_t val = d.imm_signed;
        printf("    Input:  IMM=%d\n", (int)val);
        printf("    Output: R%d = %d\n", d.r1_index, (int)val);

        set_register(d.r1_index, (int)val);
        notify_reg_change(d.r1_index, old_r1, (int)val);
        break;
    }

    /* ---- BEQZ ---- */
    case OPCODE_BEQZ: {
        int8_t r1 = d.r1_val;
        int8_t imm = d.imm_signed;
        uint16_t target = (uint16_t)((int)d.fetch_pc + 1 + (int)imm);

        printf("    Input:  R%d=%d, IMM=%d, fetch_PC=%u\n",
               d.r1_index, (int)r1, (int)imm, (unsigned)d.fetch_pc);

        if (r1 == 0) {
            set_pc((int)target);
            pipeline_flush_if_id();
            pipeline_flush_id_ex();
            printf("    Output: Branch taken, PC updated to %u\n", (unsigned)target);
        } else {
            printf("    Output: Branch not taken\n");
        }
        break;
    }

    /* ---- AND ---- */
    case OPCODE_AND: {
        int8_t a = d.r1_val, b = d.r2_val;
        int8_t result = a & b;

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)a, d.r2_index, (int)b);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        update_nz_flags(result);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- OR ---- */
    case OPCODE_OR: {
        int8_t a = d.r1_val, b = d.r2_val;
        int8_t result = a | b;

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)a, d.r2_index, (int)b);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        update_nz_flags(result);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- JR ---- */
    case OPCODE_JR: {
        int8_t r1 = d.r1_val, r2 = d.r2_val;
        uint16_t target = (uint16_t)(((int)(uint8_t)r1 << 8) | (int)(uint8_t)r2);

        printf("    Input:  R%d=%d, R%d=%d\n", d.r1_index, (int)r1, d.r2_index, (int)r2);
        printf("    Output: Jump to PC = %u\n", (unsigned)target);

        set_pc((int)target);
        pipeline_flush_if_id();
        pipeline_flush_id_ex();
        break;
    }

    /* ---- SAL ---- */
    case OPCODE_SAL: {
        int8_t val = d.r1_val;
        int    amt = (int)d.imm_unsigned;
        int8_t result;

        if (amt >= 8)
            result = 0;
        else
            result = (int8_t)((int)val << amt);

        printf("    Input:  R%d=%d, shift=%d\n", d.r1_index, (int)val, amt);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        update_nz_flags(result);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- SAR ---- */
    case OPCODE_SAR: {
        int8_t val = d.r1_val;
        int    amt = (int)d.imm_unsigned;
        int8_t result;

        if (amt >= 8)
            result = (val < 0) ? (int8_t)(-1) : (int8_t)0;
        else
            result = (int8_t)((int8_t)val >> amt);  /* arithmetic shift */

        printf("    Input:  R%d=%d, shift=%d\n", d.r1_index, (int)val, amt);
        printf("    Output: Result = %d\n", (int)result);

        set_register(d.r1_index, (int)result);
        update_nz_flags(result);

        notify_reg_change(d.r1_index, old_r1, (int)(int8_t)result);
        notify_sreg_change(old_sreg, get_sreg_full());
        break;
    }

    /* ---- LB ---- */
    case OPCODE_LB: {
        int addr = (int)d.imm_unsigned;
        int mem_val = 0;
        get_data(addr, &mem_val);
        int8_t result = (int8_t)mem_val;

        printf("    Input:  DataMem[%d]=%d\n", addr, mem_val);
        printf("    Output: R%d = %d\n", d.r1_index, (int)result);

        set_register(d.r1_index, (int)result);
        notify_reg_change(d.r1_index, old_r1, (int)result);
        break;
    }

    /* ---- SB ---- */
    case OPCODE_SB: {
        int addr = (int)d.imm_unsigned;
        int old_mem = 0;
        get_data(addr, &old_mem);
        int8_t write_val = d.r1_val;

        printf("    Input:  R%d=%d, addr=%d\n", d.r1_index, (int)write_val, addr);
        printf("    Output: DataMem[%d] = %d\n", addr, (int)write_val);

        set_data(addr, (int)write_val);
        notify_mem_change(addr, old_mem, (int)write_val);
        break;
    }

    default:
        printf("    [EX] Unknown opcode %d\n", d.opcode);
        break;
    }
}
