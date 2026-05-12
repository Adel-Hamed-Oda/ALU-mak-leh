#ifndef PARSER_H
#define PARSER_H

#include "instruction_manager.h"

/*
 * Execute the instruction held in `d` (the ID/EX pipeline register snapshot).
 * Handles all 12 opcodes, updates registers, SREG, data memory, and PC.
 * Prints the Stage EX output including register/memory change notifications.
 */
void execute_stage(ID_EX_Register d);

#endif /* PARSER_H */
