#include <stdio.h>
#include "6502-core.h"

int main(int argc, const char *argv[]) {
    struct m6502 proc;
    init_proc(&proc);

    if (argc != 2) {
        printf("enter filename\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("error opening file");
        return 1;
    }

    fread(proc.memory, MEM_SIZE, 1, file);
    fclose(file);

    disassemble(0, proc.memory, 16);
    run_emulator(&proc);
    dump_regs(&proc);

    return 0;
}


