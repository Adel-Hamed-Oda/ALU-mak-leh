#include "../include/include.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define COMMAND_PATH "../gui/command.txt"

static void sleep_for_commands(void) {
    #ifdef _WIN32
        Sleep(50);
    #else
        usleep(50000);
    #endif
}

static void write_command(const char *command) {
    FILE *file = fopen(COMMAND_PATH, "w");
    if (file) {
        fprintf(file, "%s\n", command);
        fclose(file);
    }
}

void handle_reset() {
    clk = PC = SREG = 0;

    memset(GPRS, 0, GPRS_NUM * sizeof(int8_t));
    memset(data_memory, 0, DATA_MEMORY_SIZE * sizeof(int8_t));

    current_instruction = -1;

    opcode = MEOW;
    R1 = 0;
    R2_imm = 0;

    write_everything_to_json();
}

void handle_step() {
    handleClkCycle();
}

void run_gui() {
    char command[64];

    gui_mode = 1;
    handle_reset();
    write_command("IDLE");

    while (true) {
        FILE *cmd_file = fopen(COMMAND_PATH, "r");
        if (!cmd_file) {
            sleep_for_commands();
            continue;
        }

        command[0] = '\0';
        fscanf(cmd_file, "%63s", command);
        fclose(cmd_file);

        if (strcmp(command, "STEP") == 0) {
            handle_step();
        } else if (strcmp(command, "RESET") == 0) {
            handle_reset();
        }

        write_command("IDLE");
        sleep_for_commands();
    }
}
