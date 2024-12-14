//
// Copyright 2024 Jeff Bush
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "6502-core.h"

void cmd_help(int argc, const char *argv[]);
void cmd_registers(int argc, const char *argv[]);
void cmd_disassemble(int argc, const char *argv[]);
void cmd_run(int argc, const char *argv[]);
void cmd_dump_memory(int argc, const char *argv[]);
void cmd_set_memory(int argc, const char *argv[]);
void cmd_step(int argc, const char *argv[]);

struct debug_command {
    const char *name;
    const char *help;
    void (*handler)(int argc, const char *argv[]);
} CMDS[] = {
    {"help", "List available commands", cmd_help},
    {"regs", "Dump registers", cmd_registers},
    {"dis", "Disassemble code [start_addr] [length]", cmd_disassemble},
    {"run", "Run program [address]", cmd_run},
    {"dm", "Dump memory [start addr] [length]", cmd_dump_memory},
    {"sm", "Set memory [start addr] [byte1] [byte2]...", cmd_set_memory},
    {"s", "Single step", cmd_step}
};

#define NUM_CMDS ((int) (sizeof(CMDS) / sizeof(struct debug_command)))

static struct m6502 proc;
static uint16_t next_disassemble_addr;
static uint16_t next_dump_addr;

int parse_number(const char *num) {
    if (num[0] == '$') {
        return strtol(num + 1, NULL, 16);
    } else {
        return strtol(num, NULL, 10);
    }
}

void cmd_registers(int argc, const char *argv[]) {
    dump_regs(&proc);
}

void cmd_disassemble(int argc, const char *argv[]) {
    int disassemble_len = 16;
    if (argc >= 2) {
        next_disassemble_addr = parse_number(argv[1]);
    }

    if (argc >= 3) {
        disassemble_len = parse_number(argv[2]);
    }

    next_disassemble_addr += disassemble(&proc, next_disassemble_addr,
        disassemble_len);
}

void cmd_dump_memory(int argc, const char *argv[]) {
    int dump_len = 64;
    if (argc >= 2) {
        next_dump_addr = parse_number(argv[1]);
    }

    if (argc >= 3) {
        dump_len = parse_number(argv[2]);
    }

    dump_memory(&proc, next_dump_addr, dump_len);
    next_dump_addr += dump_len;
}

void cmd_set_memory(int argc, const char *argv[]) {
    if (argc < 3) {
        printf("Too few arguments\n");
        return;
    }

    uint16_t base_addr = parse_number(argv[1]);
    for (int i = 2; i < argc; i++) {
        proc.memory[base_addr + i - 2] = parse_number(argv[i]) & 0xff;
    }
}

void cmd_run(int argc, const char *argv[]) {
    if (argc >= 2) {
        proc.pc = parse_number(argv[1]);
    }

    run_emulator(&proc, 0);
    printf("Halted\n");
    dump_regs(&proc);
}

void cmd_help(int argc, const char *argv[]) {
    printf("commands:\n");
    for (int i = 0; i < NUM_CMDS; i++) {
        printf("%10s   %s\n", CMDS[i].name, CMDS[i].help);
    }
}

void cmd_step(int argc, const char *argv[]) {
    run_emulator(&proc, 1);
}

void load_program(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("error opening file");
        exit(1);
    }

    fread(proc.memory, MEM_SIZE, 1, file);
    fclose(file);
}

void dispatch_command(char *command) {
    const int MAX_ARGS = 16;
    const char *argv[MAX_ARGS];
    int argc;

    argv[0] = strtok(command, " ");
    if (argv[0] == NULL) {
        return;
    }

    for (argc = 1; argc < MAX_ARGS; argc++) {
        argv[argc] = strtok(0, " ");
        if (!argv[argc]) {
            break;
        }
    }

    for (int i = 0; ; i++) {
        if (i == NUM_CMDS) {
            printf("unknown command %s\n", argv[0]);
            break;
        }

        if (strcmp(argv[0], CMDS[i].name) == 0) {
            CMDS[i].handler(argc, argv);
            break;
        }
    }
}

void monitor_loop() {
    while (1) {
        printf("* ");
        char command[128];
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strlen(command) - 1] = '\0'; // strip newline
        dispatch_command(command);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int debug = 0;

    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd':
                debug = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-d] <binary file>\n",
                        argv[0]);
                exit(1);
        }
    }

    if (optind >= argc) {
        printf("Missing binary filename\n");
        exit(1);
    }

    init_proc(&proc);
    load_program(argv[optind]);
    if (debug) {
        monitor_loop();
    } else {
        run_emulator(&proc, 0);
    }

    return 0;
}


