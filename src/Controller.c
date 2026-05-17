#include "../include/include.h"

int8_t GPRS[GPRS_NUM] = {0};
int8_t SREG = 0;
int16_t PC = 0;

int16_t current_instruction = -1;
int16_t executing_instruction = -1;
OPCODE opcode = MEOW;
int8_t R1 = 0;
int8_t R2_imm = 0;

int clk = 0;
int gui_mode = 0;

void enter() {
    printf("\nPress [ENTER] to go to the next clk cycle\n");
    int c = getchar();

    while (c == 'R' || c == 'D' || c == 'I') {
        if (c == 'R') {
            int i;
            printf("Enter until the register block you want to view: ");
            scanf("%i", &i);

            display_register_content(i);
        } else if (c == 'D') {
            int i;
            printf("Enter until the memory block you want to view: ");
            scanf("%i", &i);

            display_memory_content(i);
        } else if (c == 'I') {
            int i;
            printf("Enter until the memory block you want to view: ");
            scanf("%i", &i);

            display_instruction_content(i);
        }else {
            break;
        }

        c = getchar();
    }
}

static int16_t encode_pipeline_instruction(OPCODE stage_opcode, int8_t stage_R1, int8_t stage_R2_imm) {
    if (stage_opcode == MEOW) {
        return -1;
    }

    return (int16_t)(((stage_opcode & 0x0F) << 12) | ((stage_R1 & 0x3F) << 6) | (stage_R2_imm & 0x3F));
}

void handleClkCycle() {
    OPCODE cur_opcode = opcode;
    executing_instruction = encode_pipeline_instruction(opcode, R1, R2_imm);
    execute();

    if (cur_opcode == JR || (GPRS[R1] == 0 && cur_opcode == BEQZ)) {        
        write_everything_to_json();
        if (!gui_mode) {
            enter();
        }
        clk++;

        return;
    }

    decode();
    fetch();

    write_everything_to_json();
    if (!gui_mode) {
        enter();
    }
    clk++;

    return;
}

int program_finished() {
    return get_instr(PC) == -1 && opcode == MEOW && current_instruction == -1;
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "--gui") == 0) {
        printf("To view the GUI:\n1) Open a new terminal\n2) Run: python gui/server.py\n");
        run_gui();
        return 0;
    }

    readProgram("program_1.txt");
    clk = 0;
    write_everything_to_json();

    while (true) {
        if (program_finished()) {
            break;
        }
        handleClkCycle();
    }

    printf("[Cycle: %i]: PROGRAM FINISHED EXECUTNG\n", clk);
}

/*
gcc Controller.c data_memory.c executer.c instruction_memory.c methods.c parser.c to_json.c gui.c -o meow.exe
 */
