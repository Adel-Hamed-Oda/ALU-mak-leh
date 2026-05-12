#include "includes.h"
#include "src/memory/instruction_memory.h"
#include "src/memory/data_memory.h"
#include "src/registers/register.h"
#include "src/instructions/instruction_manager.h"
#include "src/instructions/parser.h"
#include "src/programs/assembler.h"

/* -----------------------------------------------------------------------
 * Cycle-level snapshots (set at the top of each cycle)
 * ----------------------------------------------------------------------- */
static IF_ID_Register old_if_id;
static ID_EX_Register old_id_ex;

/* -----------------------------------------------------------------------
 * Stage: Instruction Fetch
 * Reads InstructionMemory[PC], stores in if_id, increments PC.
 * ----------------------------------------------------------------------- */
static void do_fetch(void) {
    int pc = get_pc();

    printf("  Stage IF:\n");

    if (pc >= num_instructions_loaded) {
        /* Nothing left to fetch — insert bubble */
        if_id.valid           = 0;
        if_id.raw_instruction = 0;
        if_id.fetch_pc        = (uint16_t)pc;
        printf("    Instruction: No instruction (bubble)\n");
        printf("    Input:  PC = %d\n", pc);
        printf("    Output: (no instruction at this address)\n");
        return;
    }

    int raw_int = 0;
    fetch_instruction(pc, &raw_int);
    int16_t raw = (int16_t)raw_int;

    if_id.raw_instruction = raw;
    if_id.fetch_pc        = (uint16_t)pc;
    if_id.valid           = 1;

    increment_pc();

    char instr_str[32];
    char bin_str[17];
    format_instruction_str(raw, instr_str);
    int16_to_binary_str(raw, bin_str);

    printf("    Instruction: %s\n", instr_str);
    printf("    Input:  PC = %d\n", pc);
    printf("    Output: Fetched instruction = %s\n", bin_str);
}

/* -----------------------------------------------------------------------
 * Stage: Instruction Decode
 * Decodes old_if_id, reads registers, detects data hazards.
 * If a stall is needed: flushes id_ex, restores if_id, undoes PC++.
 * ----------------------------------------------------------------------- */
static void do_decode(void) {
    printf("  Stage ID:\n");

    if (!old_if_id.valid) {
        id_ex.valid = 0;
        printf("    Instruction: No instruction (bubble)\n");
        return;
    }

    int16_t raw      = old_if_id.raw_instruction;
    int  opcode      = (raw >> 12) & 0xF;
    int  r1_index    = (raw >> 6)  & 0x3F;
    int  r2_index    =  raw        & 0x3F;
    int  imm_raw     = r2_index;
    int8_t imm_s     = sign_extend_6to8(imm_raw);
    uint8_t imm_u    = (uint8_t)(imm_raw & 0x3F);

    /* ---- Data hazard detection (stall) ---- */
    if (old_id_ex.valid && instruction_writes_register(old_id_ex.opcode)) {
        int dest  = old_id_ex.r1_index;
        int stall = 0;

        if (instruction_reads_r1(opcode) && r1_index == dest)
            stall = 1;
        if (!stall && is_r_format(opcode) && r2_index == dest)
            stall = 1;

        if (stall) {
            char instr_str[32];
            format_instruction_str(raw, instr_str);
            printf("    Instruction: %s  [DATA HAZARD STALL]\n", instr_str);
            printf("    Hazard: instruction in EX (R%d) conflicts with source register\n",
                   dest);
            printf("    Action: bubble inserted into EX; instruction will retry next cycle\n");

            /* Insert bubble, re-present stalled instruction, undo PC increment */
            pipeline_flush_id_ex();
            if_id    = old_if_id;
            set_pc(get_pc() - 1);
            return;
        }
    }

    /* ---- Normal decode ---- */
    int r1_val_int = 0, r2_val_int = 0;
    get_register(r1_index, &r1_val_int);
    get_register(r2_index, &r2_val_int);

    id_ex.opcode       = opcode;
    id_ex.r1_index     = r1_index;
    id_ex.r2_index     = r2_index;
    id_ex.r1_val       = (int8_t)r1_val_int;
    id_ex.r2_val       = (int8_t)r2_val_int;
    id_ex.imm_signed   = imm_s;
    id_ex.imm_unsigned = imm_u;
    id_ex.fetch_pc     = old_if_id.fetch_pc;
    id_ex.valid        = 1;

    char instr_str[32];
    char bin_str[17];
    format_instruction_str(raw, instr_str);
    int16_to_binary_str(raw, bin_str);

    printf("    Instruction: %s\n", instr_str);
    printf("    Input:  Instruction bits = %s, R%d = %d, R%d = %d\n",
           bin_str, r1_index, r1_val_int, r2_index, r2_val_int);
    printf("    Output: opcode=%d, R1_index=%d, R1_val=%d, "
           "R2_index=%d, R2_val=%d, IMM=%d\n",
           opcode, r1_index, r1_val_int, r2_index, r2_val_int, (int)imm_s);
}

/* -----------------------------------------------------------------------
 * Stage: Execute
 * Delegates to execute_stage() in parser.c.
 * ----------------------------------------------------------------------- */
static void do_execute(void) {
    execute_stage(old_id_ex);
}

/* -----------------------------------------------------------------------
 * Final state dump
 * ----------------------------------------------------------------------- */
static void print_final_state(void) {
    dump_all_registers();
    printf("\n");
    printf("=== Final Instruction Memory ===\n");
    /* Print only the loaded instructions */
    for (int i = 0; i < num_instructions_loaded; i++) {
        int raw_int = 0;
        fetch_instruction(i, &raw_int);
        int16_t raw = (int16_t)raw_int;
        char bin_str[17];
        char instr_str[32];
        int16_to_binary_str(raw, bin_str);
        format_instruction_str(raw, instr_str);
        printf("[%4d]: %s  ; %s\n", i, bin_str, instr_str);
    }
    printf("(addresses %d..1023: 0000000000000000)\n", num_instructions_loaded);
    printf("\n");
    print_data_memory();
}

/* -----------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------- */
int main(int argc, char *argv[]) {
    const char *filepath = (argc > 1) ? argv[1] : "programs/test0.txt";

    /* Initialise */
    initialize_registers();
    initialize_instruction_memory();
    initialize_data_memory();
    pipeline_init();

    /* Load & assemble */
    if (FAIL(load_program_from_file((char *)filepath))) {
        fprintf(stderr, "Failed to load program from '%s'\n", filepath);
        return 1;
    }

    printf("Loaded %d instructions from '%s'\n\n", num_instructions_loaded, filepath);

    if (num_instructions_loaded == 0) {
        printf("No instructions to simulate.\n");
        return 0;
    }

    /* Simulation loop */
    int cycle_count = 0;
    while (get_pc() < num_instructions_loaded || if_id.valid || id_ex.valid) {
        cycle_count++;
        printf("Clock Cycle #%d:\n", cycle_count);

        /* Snapshot pipeline state before any stage runs */
        old_if_id = if_id;
        old_id_ex = id_ex;

        do_fetch();
        printf("\n");
        do_decode();
        printf("\n");
        do_execute();
        printf("\n");
    }

    printf("Simulation complete: %d clock cycles.\n\n", cycle_count);
    print_final_state();
    return 0;
}
