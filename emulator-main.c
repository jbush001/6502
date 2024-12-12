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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "6502-core.h"

void cmd_registers(int argc, const char *argv[]);
void cmd_disassemble(int argc, const char *argv[]);
void cmd_run(int argc, const char *argv[]);
void cmd_help(int argc, const char *argv[]);
void cmd_dump_memory(int argc, const char *argv[]);

struct debug_command {
    const char *name;
    void (*handler)(int argc, const char *argv[]);
} CMDS[] = {
    {"regs", cmd_registers},
    {"dis", cmd_disassemble},
    {"run", cmd_run},
    {"help", cmd_help},
    {"dm", cmd_dump_memory}
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

void cmd_run(int argc, const char *argv[]) {
    run_emulator(&proc);
    printf("Halted\n");
    dump_regs(&proc);
}

void cmd_help(int argc, const char *argv[]) {
    printf("commands:\n");
    for (int i = 0; i < NUM_CMDS; i++) {
        printf("%s\n", CMDS[i].name);
    }
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
    const char *toks[MAX_ARGS];
    int argn;

    toks[0] = strtok(command, " ");
    if (toks[0] == NULL) {
        return;
    }

    for (argn = 1; argn < MAX_ARGS; argn++) {
        toks[argn] = strtok(0, " ");
        if (!toks[argn]) {
            break;
        }
    }

    for (int i = 0; ; i++) {
        if (i == NUM_CMDS) {
            printf("unknown command %s\n", toks[0]);
            break;
        }

        if (strcmp(toks[0], CMDS[i].name) == 0) {
            CMDS[i].handler(argn, toks);
            break;
        }
    }
}

int main(int argc, const char *argv[]) {
    init_proc(&proc);

    if (argc != 2) {
        printf("enter filename\n");
        return 1;
    }

    load_program(argv[1]);

    char command[128];
    while (1) {
        printf("* ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strlen(command) - 1] = '\0'; // strip newline
        dispatch_command(command);
    }

    return 0;
}


