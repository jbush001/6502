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
#include "6502-core.h"

#define TEST_EQ(x, y) { \
    if ((x) != (y)) { printf("Test failed (line %d): $%x != $%x\n", \
        __LINE__, (x), (y)); exit(1); } }

void test_ld() {
    struct m6502 proc;
    init_proc(&proc);

    // Immediate
    proc.memory[0] = 0xa9; // LDA #$24
    proc.memory[1] = 0x24;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x24);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Test N flag
    proc.memory[1] = 0xc5;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xc5);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Test Z flag
    proc.memory[1] = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0);
    TEST_EQ(proc.z, 1);
    TEST_EQ(proc.n, 0);

    // Absolute
    proc.memory[0] = 0xad; // LDA $100
    proc.memory[1] = 0;
    proc.memory[2] = 1;
    proc.memory[256] = 0xa9;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xa9);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Zero page
    proc.memory[0] = 0xa5; // LDA $20
    proc.memory[1] = 0x20;
    proc.memory[0x20] = 0x52;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x52);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Absolute indexed
    proc.memory[0] = 0xbd; // LDA $01,X
    proc.memory[1] = 0x01;
    proc.memory[2] = 0x01;
    proc.x = 3;
    proc.memory[0x104] = 0x8f;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x8f);

    // Absolute indexed
    proc.memory[0] = 0xb9; // LDA $202,Y
    proc.memory[1] = 0x02;
    proc.memory[2] = 0x02;
    proc.y = 7;
    proc.memory[0x209] = 0x49;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x49);

    // Zero page indexed
    proc.memory[0] = 0xb5; // LDA $20,X
    proc.memory[1] = 0x20;
    proc.memory[2] = 0;
    proc.x = 0x12;
    proc.memory[0x32] = 0x49;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x49);

    // Zero page indirect indexed
    proc.memory[0] = 0xb1; // LDA ($20), Y
    proc.memory[1] = 0x20;
    proc.memory[2] = 0;
    proc.y = 0x3;
    proc.memory[0x20] = 0x04;
    proc.memory[0x21] = 0x3;
    proc.memory[0x307] = 0xce;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xce);

    // Zero page indexed indirect
    proc.memory[0] = 0xa1; // LDA ($13, X)
    proc.memory[1] = 0x13;
    proc.memory[2] = 0;
    proc.x = 0x25;
    proc.memory[0x38] = 0x31;
    proc.memory[0x39] = 0x4;
    proc.memory[0x431] = 0xf5;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xf5);

    // LDX. Group 2.
    proc.memory[0] = 0xa2; // LDX #$24
    proc.memory[1] = 0x24;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.x, 0x24);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Absolute
    proc.memory[0] = 0xae; // LDX $100
    proc.memory[1] = 0;
    proc.memory[2] = 1;
    proc.memory[256] = 0xa9;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0xa9);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Zero page
    proc.memory[0] = 0xa6; // LDX $20
    proc.memory[1] = 0x20;
    proc.memory[2] = 0;
    proc.memory[0x20] = 0x52;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0x52);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // LDY. This is a group 3 instruction with a different
    // encoding for addressing modes.
    // Immediate
    proc.memory[0] = 0xa0; // LDY #$24
    proc.memory[1] = 0x24;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.y, 0x24);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Absolute
    proc.memory[0] = 0xac; // LDY $100
    proc.memory[1] = 0;
    proc.memory[2] = 1;
    proc.memory[256] = 0xa9;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0xa9);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Zero page
    proc.memory[0] = 0xa4; // LDY $20
    proc.memory[1] = 0x20;
    proc.memory[2] = 0;
    proc.memory[0x20] = 0x52;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0x52);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Absolute indexed
    proc.memory[0] = 0xbc; // LDY $101,X
    proc.memory[1] = 0x01;
    proc.memory[2] = 0x01;
    proc.x = 3;
    proc.memory[0x104] = 0x8f;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0x8f);

    // Zero page indexed
    proc.memory[0] = 0xb4; // LDY $20,X
    proc.memory[1] = 0x20;
    proc.memory[2] = 0;
    proc.x = 0x12;
    proc.memory[0x32] = 0x49;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0x49);
}

// XXX I don't hit all of the addressing modes here, since this uses the same
// code to resolve the address as the LD instructions above.
void test_st() {
    struct m6502 proc;
    init_proc(&proc);

    // Absolute
    proc.a = 0x7b;
    proc.memory[0] = 0x8d; // STA $120
    proc.memory[1] = 0x20;
    proc.memory[2] = 0x01;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x120], 0x7b);

    // Zero page
    proc.a = 0xef;
    proc.memory[0] = 0x85; // STA $21
    proc.memory[1] = 0x21;
    proc.memory[2] = 0x00;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x21], 0xef);

    // STX
    proc.x = 0x22;
    proc.memory[0] = 0x86; // STX $120
    proc.memory[1] = 0x40;
    proc.memory[2] = 0x00;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x40], 0x22);

    // STY
    proc.y = 0x45;
    proc.memory[0] = 0x84; // STY $120
    proc.memory[1] = 0x41;
    proc.memory[2] = 0x00;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x41], 0x45);
}

// XXX this also doesn't hit all addressing modes, as above.
// It is more focused on proper flag handling behavior.
void test_adc() {
    struct m6502 proc;
    init_proc(&proc);

    // No overflow, no carry out or in
    proc.memory[0] = 0x69; // ADC #$13
    proc.memory[1] = 0x13;
    proc.a = 0x27;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x3a);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.v, 0);

    // Carry in
    proc.pc = 0;
    proc.c = 1;
    proc.a = 0x27;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x3b);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.v, 0);

    // Carry out, no overflow
    proc.pc = 0;
    proc.memory[0] = 0x69; // ADC #192
    proc.memory[1] = 192;
    proc.a = 127;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 63);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.v, 0);

    // Carry out, overflow
    proc.pc = 0;
    proc.memory[0] = 0x69; // ADC #192
    proc.memory[1] = 192;
    proc.a = 128;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 64);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.v, 1);

    // Overflow, no carry out
    proc.pc = 0;
    proc.memory[0] = 0x69; // ADC #192
    proc.memory[1] = 126;
    proc.a = 3;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 129);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.v, 1);

    // Zero result
    proc.pc = 0;
    proc.memory[0] = 0x69; // ADC #-23
    proc.memory[1] = 233;
    proc.a = 23;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0);
    TEST_EQ(proc.z, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.v, 0);
}

// XXX SBC shares code with ADC, but inverts the second operand.
void test_sbc() {
    struct m6502 proc;
    init_proc(&proc);

    // No overflow, no carry out or in
    proc.memory[0] = 0xe9; // SBC #$13
    proc.memory[1] = 0x13;
    proc.a = 0x27;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x14);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.c, 1); // Borrow is reversed vs. adc
    TEST_EQ(proc.v, 0);
}

void test_branch() {
    struct m6502 proc;
    init_proc(&proc);

    // Absolute, unconditional
    proc.memory[0] = 0x4c; // JMP $103
    proc.memory[1] = 0x03;
    proc.memory[2] = 0x01;
    proc.memory[3] = 0;
    proc.memory[0x103] = 0; // BRK
    run_emulator(&proc);
    TEST_EQ(proc.pc, 0x104);

    // Indirect, unconditional
    proc.pc = 0;
    proc.memory[0] = 0x6c; // JMP ($20)
    proc.memory[1] = 0x20;
    proc.memory[2] = 0x00;
    proc.memory[3] = 0;
    proc.memory[0x20] = 0x21; // Indirect address $121
    proc.memory[0x21] = 0x1;
    proc.memory[0x121] = 0; // BRK
    run_emulator(&proc);
    TEST_EQ(proc.pc, 0x122);

    // BCS, taken
    proc.pc = 0;
    proc.memory[0] = 0xb0; // BCS +3
    proc.memory[1] = 0x03;
    proc.memory[2] = 0x00; // BRK
    proc.memory[3] = 0x00;
    proc.memory[4] = 0x00;
    proc.memory[5] = 0x00;
    proc.c = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BCS, not taken
    proc.pc = 0;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BCC, taken
    proc.pc = 0;
    proc.memory[0] = 0x90; // BCC +3
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BCC, not taken
    proc.pc = 0;
    proc.memory[0] = 0x90; // BCC +3
    proc.c = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BVS, taken
    proc.pc = 0;
    proc.memory[0] = 0x70; // BVS +3
    proc.v = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BVS, not taken
    proc.pc = 0;
    proc.v = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BVC, taken
    proc.pc = 0;
    proc.memory[0] = 0x50; // BVC +3
    proc.v = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BVC, not taken
    proc.pc = 0;
    proc.v = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BMI, taken
    proc.pc = 0;
    proc.memory[0] = 0x30; // BMI +3
    proc.n = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BMI, not taken
    proc.pc = 0;
    proc.n = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BPL, taken
    proc.pc = 0;
    proc.memory[0] = 0x10; // BPL +3
    proc.n = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BPL, not taken
    proc.pc = 0;
    proc.n = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BEQ, taken
    proc.pc = 0;
    proc.memory[0] = 0xf0; // BEQ +3
    proc.z = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BEQ, not taken
    proc.pc = 0;
    proc.z = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);

    // BNE, taken
    proc.pc = 0;
    proc.memory[0] = 0xd0; // BNE +3
    proc.z = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 6);

    // BNE, not taken
    proc.pc = 0;
    proc.z = 1;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 3);
}

void test_shifts() {
    struct m6502 proc;
    init_proc(&proc);

    // Accumulator, no carry in, carry out
    proc.memory[0] = 0x2a; // ROL
    proc.memory[1] = 0;
    proc.c = 0;
    proc.a = 0x9c;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x38);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);

    // Carry in, carry out
    proc.pc = 0;
    proc.c = 1;
    proc.a = 0x9c;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x39);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);

    // No carry in, no carry out
    proc.pc = 0;
    proc.c = 0;
    proc.a = 0x7c;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xf8);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    // Memory location, no carry in, carry out
    proc.memory[0] = 0x2e; // ROL $100
    proc.memory[1] = 0;
    proc.memory[2] = 1;
    proc.memory[3] = 0;
    proc.memory[0x100] = 0xe4;
    proc.pc = 0;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x100], 0xc8);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    // Memory location, carry in, no carry out
    proc.memory[0x100] = 0x75;
    proc.pc = 0;
    proc.c = 1;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x100], 0xeb);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    // Location gets set to zero
    proc.memory[0x100] = 0x80;
    proc.pc = 0;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0x100], 0);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 1);

    // Test ROR
    proc.memory[0] = 0x6a; // ROR
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.c = 1;
    proc.a = 0x9d;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0xce);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    // LSR does not shift in the carry
    proc.memory[0] = 0x4a; // LSR
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.c = 1;
    proc.a = 0x9d;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x4e);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);

    // ASL does not shift in the carry
    proc.memory[0] = 0xa; // ASL
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.c = 1;
    proc.a = 0x9d;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x3a);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);
}

void test_logical() {
    struct m6502 proc;
    init_proc(&proc);

    proc.memory[0] = 0x29; // AND #$a5
    proc.memory[1] = 0xa5;
    proc.a = 0xc3;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x81);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0x9; // ORA #$91
    proc.memory[1] = 0x91;
    proc.a = 0x18;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x99);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0x49; // EOR #$91
    proc.memory[1] = 0x91;
    proc.a = 0x80;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0x11);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);
}

void test_jsr_rts() {
    struct m6502 proc;
    init_proc(&proc);

    // Call
    proc.memory[0] = 0xea; // NOP
    proc.memory[1] = 0xea;
    proc.memory[2] = 0x20; // JSR $30
    proc.memory[3] = 0x30;
    proc.memory[4] = 0; // BRK
    proc.memory[5] = 0; // BRK
    proc.memory[0x30] = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 0x31);
    TEST_EQ(proc.s, 0xfd);
    TEST_EQ(proc.memory[0x1fe], 0x3);
    TEST_EQ(proc.memory[0x1ff], 0x0);

    // Return
    proc.memory[0] = 0x60; // RTS
    proc.memory[1] = 0;
    proc.memory[2] = 0;
    proc.memory[3] = 0;
    proc.memory[4] = 0;
    proc.s = 0xc0;
    proc.memory[0x1c1] = 0x34;
    proc.memory[0x1c2] = 0x12;
    proc.memory[0x1234] = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.pc, 0x1235);
    TEST_EQ(proc.s, 0xc2);
}

void test_stack() {
    struct m6502 proc;
    init_proc(&proc);

    // Push
    proc.memory[0] = 0x48; // PHA
    proc.memory[1] = 0;
    proc.a = 0xe2;
    run_emulator(&proc);
    TEST_EQ(proc.s, 0xfe);
    TEST_EQ(proc.memory[0x1ff], 0xe2);

    // Pop
    proc.memory[0] = 0x68; // PLA
    proc.a = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.s, 0xff);
    TEST_EQ((uint8_t) proc.a, 0xe2);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);
}

void test_transfer() {
    struct m6502 proc;
    init_proc(&proc);

    proc.memory[0] = 0xaa; // TAX
    proc.memory[1] = 0;
    proc.a = 0x8d;
    proc.x = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x8d);
    TEST_EQ((uint8_t) proc.x, 0x8d);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    // Transfer a zero
    proc.a = 0;
    proc.x = 0x8d;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.a, 0);
    TEST_EQ(proc.x, 0);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 1);

    proc.memory[0] = 0x8a; // TXA
    proc.memory[1] = 0;
    proc.a = 0;
    proc.x = 0x8d;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x8d);
    TEST_EQ((uint8_t) proc.x, 0x8d);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0x9a; // TXS
    proc.memory[1] = 0;
    proc.s = 0;
    proc.x = 0xd6;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.s, 0xd6);
    TEST_EQ((uint8_t) proc.x, 0xd6);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0xba; // TSX
    proc.memory[1] = 0;
    proc.s = 0x6b;
    proc.x = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.s, 0x6b);
    TEST_EQ((uint8_t) proc.x, 0x6b);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0xa8; // TAY
    proc.memory[1] = 0;
    proc.a = 0x8f;
    proc.y = 0;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x8f);
    TEST_EQ((uint8_t) proc.y, 0x8f);
    TEST_EQ(proc.n, 1);
    TEST_EQ(proc.z, 0);

    proc.memory[0] = 0x98; // TYA
    proc.memory[1] = 0;
    proc.a = 0;
    proc.y = 0x14;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.a, 0x14);
    TEST_EQ((uint8_t) proc.y, 0x14);
    TEST_EQ(proc.n, 0);
    TEST_EQ(proc.z, 0);
}

void test_inc_dec() {
    struct m6502 proc;
    init_proc(&proc);

    // Increment X
    proc.memory[0] = 0xe8; // INX
    proc.memory[1] = 0;
    proc.x = 0x23;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0x24);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Increment to negative
    proc.x = 0x7f;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0x80);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Increment to zero
    proc.x = 0xff;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0x0);
    TEST_EQ(proc.z, 1);
    TEST_EQ(proc.n, 0);

    // Decrement X
    proc.memory[0] = 0xca; // DEX
    proc.memory[1] = 0;
    proc.x = 0x37;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.x, 0x36);

    // Increment Y
    proc.memory[0] = 0xc8; // INY
    proc.memory[1] = 0;
    proc.y = 0x23;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0x24);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 0);

    // Decrement Y
    proc.memory[0] = 0x88; // DEY
    proc.memory[1] = 0;
    proc.y = 0x37;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ((uint8_t) proc.y, 0x36);

    // Increment memory location
    proc.memory[0] = 0xe6; // INC $f0
    proc.memory[1] = 0xf0;
    proc.memory[2] = 0;
    proc.memory[0xf0] = 0x82;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0xf0], 0x83);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.n, 1);

    // Decrement memory location
    proc.memory[0] = 0xc6; // DEC $f0
    proc.memory[1] = 0xf7;
    proc.memory[2] = 0;
    proc.memory[0xf7] = 0x1;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.memory[0xf7], 0);
    TEST_EQ(proc.z, 1);
    TEST_EQ(proc.n, 0);
}

void test_set_clear_flags() {
    struct m6502 proc;
    init_proc(&proc);

    proc.memory[0] = 0x18; // CLC
    proc.memory[1] = 0;
    proc.c = 1;
    run_emulator(&proc);
    TEST_EQ(proc.c, 0);

    proc.memory[0] = 0x38; // SEC
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.c = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 1);

    proc.memory[0] = 0xf8; // SED
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.d = 0;
    run_emulator(&proc);
    TEST_EQ(proc.d, 1);

    proc.memory[0] = 0xd8; // CLD
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.d = 1;
    run_emulator(&proc);
    TEST_EQ(proc.d, 0);

    proc.memory[0] = 0xb8; // CLV
    proc.memory[1] = 0;
    proc.pc = 0;
    proc.v = 1;
    run_emulator(&proc);
    TEST_EQ(proc.v, 0);
}

// Ensure comparisons don't look at carry in.
void test_compare() {
    struct m6502 proc;
    init_proc(&proc);

    // A < operand
    proc.memory[0] = 0xc9; // CMP #$77
    proc.memory[1] = 0x77;
    proc.memory[2] = 0;
    proc.a = 0x76;
    run_emulator(&proc);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.a, 0x76);

    // A = operand
    proc.a = 0x77;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.z, 1);
    TEST_EQ(proc.a, 0x77);

    // A > operand
    proc.a = 0x78;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.a, 0x78);

    // X < operand
    proc.memory[0] = 0xe0; // CPX #$77
    proc.x = 0x76;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.x, 0x76);

    // X > operand
    proc.x = 0x78;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.x, 0x78);

    // Y < operand
    proc.memory[0] = 0xc0; // CPY #$77
    proc.y = 0x76;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 0);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.y, 0x76);

    // Y > operand
    proc.y = 0x78;
    proc.pc = 0;
    run_emulator(&proc);
    TEST_EQ(proc.c, 1);
    TEST_EQ(proc.z, 0);
    TEST_EQ(proc.y, 0x78);
}

int main() {
    test_ld();
    test_st();
    test_adc();
    test_sbc();
    test_branch();
    test_shifts();
    test_logical();
    test_jsr_rts();
    test_stack();
    test_transfer();
    test_inc_dec();
    test_set_clear_flags();
    test_compare();

    printf("PASS\n");
    return 0;
}
