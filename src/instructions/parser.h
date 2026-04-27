// the parser should only use memory and registers to do
// its bidding. Any other files shouldn't be used, it should also return SUCCESS or FAILURE
int execute_instruction(int opcode, int r1, int r2);