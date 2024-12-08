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


