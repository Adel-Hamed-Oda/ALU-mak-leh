// Now this is gonna be very awkward, bas here is the thing
// Since there is only one program, I can just clear both memories every time we load
// Which means, that at assembly time, I can start from index 0 and work my way from up 

// also there are only 2 opcodes where I have to deal with variables, otherwise I can just
// translate the register number directly to the instruction

// The format is probably R1 for register
EXCEPTION load_program_from_file(char *file_name);

// You guys shouldn't need to do this but IDK
EXCEPTION assemble_program_from_file(char *file_name, int *out_instructions, int *out_instruction_count);