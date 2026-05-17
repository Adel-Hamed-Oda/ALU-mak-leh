#include "../include/include.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define COMMAND_PATH "../gui/command.txt"
#define DEFAULT_PROGRAM "program_1.txt"

static char current_program[64] = DEFAULT_PROGRAM;

static void sleep_for_commands(void) {
    #ifdef _WIN32
        Sleep(50);
    #else
        usleep(50000);
    #endif
}
static void sleep_for_run(void) {
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000000);
    #endif
}

static void write_command(const char *command) {
    FILE *file = fopen(COMMAND_PATH, "w");
    if (file) {
        fprintf(file, "%s\n", command);
        fclose(file);
    }
}

void handle_load(const char *filename) {
    clk = PC = SREG = 0;

    memset(GPRS, 0, GPRS_NUM * sizeof(int8_t));
    memset(data_memory, 0, DATA_MEMORY_SIZE * sizeof(int8_t));

    current_instruction = -1;

    opcode = MEOW;
    R1 = 0;
    R2_imm = 0;

    clear_instr();
    readProgram((char *)filename);
    write_everything_to_json();
}

void handle_reset() {
    handle_load(current_program);
}

void handle_step() {
    if (program_finished()) {
        return;
    }

    handleClkCycle();
}

void run_gui() {
    char command[64];
    char filename[64];

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
        filename[0] = '\0';
        fscanf(cmd_file, "%63s", command);

        if (strcmp(command, "LOAD") == 0) {
            if (fscanf(cmd_file, " %63[^\r\n]", filename) != 1) {
                filename[0] = '\0';
            }
        }

        fclose(cmd_file);

        if (strcmp(command, "STEP") == 0) {
            handle_step();
            write_command("IDLE");
        } else if (strcmp(command, "RESET") == 0) {
            handle_reset();
            write_command("IDLE");
        } else if (strcmp(command, "LOAD") == 0 && filename[0] != '\0') {
            strncpy(current_program, filename, sizeof(current_program) - 1);
            current_program[sizeof(current_program) - 1] = '\0';
            handle_load(current_program);
            write_command("IDLE");
        } else if (strcmp(command, "RUN") == 0) {
            if (program_finished()) {
                write_command("IDLE");
            } else {
                sleep_for_run();
                handle_step();

                if (program_finished()) {
                    write_command("IDLE");
                }
            }
        } else if (strcmp(command, "PAUSE") == 0) {
            write_command("IDLE");
        }

        
        sleep_for_commands();
    }
}
