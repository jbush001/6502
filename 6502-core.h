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

#ifndef __6502_CORE_H
#define __6502_CORE_H

#include <stdint.h>

#define MEM_SIZE 0x10000

struct m6502 {
    int8_t a;
    uint8_t x;
    uint8_t y;
    uint16_t s;
    uint16_t pc;

    // flags
    uint8_t n : 1;
    uint8_t v : 1;
    uint8_t b : 1;
    uint8_t d : 1;
    uint8_t i : 1;
    uint8_t z : 1;
    uint8_t c : 1;

    uint8_t *memory;
    int halt;
};

void run_emulator(struct m6502 *proc);
void init_proc(struct m6502 *proc);
void disassemble(uint16_t base_addr, uint8_t *memory, int length);
void dump_regs(struct m6502 *proc);

#endif