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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"

#define MEM_SIZE 0x10000

struct m6502 {
    int8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t s;
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
    int running;
};

uint8_t read8(struct m6502 *proc, uint16_t addr) {
    // XXX bounds check
    return proc->memory[addr];
}

void write8(struct m6502 *proc, uint16_t addr, uint8_t val) {
    // XXX bounds check
    proc->memory[addr] = val;
}

uint16_t read16(struct m6502 *proc, uint16_t addr) {
    // XXX bounds check
    return proc->memory[addr] | (proc->memory[addr + 1] << 8);
}

uint16_t get_addr(struct m6502 *proc, enum address_mode mode) {
    switch (mode) {
        case IND_ZERO_PAGE_X: { // ($hh, X)
            unsigned short addr = read8(proc, proc->pc++) + proc->x;
            return read16(proc, addr);
        }

        case ZERO_PAGE: // $hh
            return read8(proc, proc->pc++);

        case ABSOLUTE: { // $hhhh
            unsigned short addr = read16(proc, proc->pc);
            proc->pc += 2;
            return addr;
        }

        case IND_ZERO_PAGE_Y: // ($hh), y
            return read16(proc, read8(proc, proc->pc++)) + proc->y;

        case ZERO_PAGE_X: // $hh, X
            return read8(proc, proc->pc++) + proc->x;

        case ABSOLUTE_X: { // $hhhh, X
            unsigned short addr = read16(proc, proc->pc);
            proc->pc += 2;
            return addr + proc->x;
        }

        case ABSOLUTE_Y: { // $hhhh, Y
            unsigned short addr = read16(proc, proc->pc);
            proc->pc += 2;
            return addr + proc->y;
        }

        case IMPLIED:
        default:
            return 0;
    }
}

uint8_t get_operand(struct m6502 *proc, enum address_mode mode) {
    // This shouldn't happen, because an implied instruction handler should
    // not call get_operand.
    assert(mode != IMPLIED);

    if (mode == IMMEDIATE) {
        return read8(proc, proc->pc++);
    } else {
        return read8(proc, get_addr(proc, mode));
    }
}

void set_zn(struct m6502 *proc) {
    proc->n = (proc->a >> 7) & 1;
    proc->z = (proc->a & 0xff) == 0;
}

void inst_ROR(struct m6502 *proc, enum address_mode mode) {
}

void inst_LSR(struct m6502 *proc, enum address_mode mode) {
}

void inst_ASL(struct m6502 *proc, enum address_mode mode) {
}

void inst_CMP(struct m6502 *proc, enum address_mode mode) {
}

void inst_EOR(struct m6502 *proc, enum address_mode mode) {
}

void inst_AND(struct m6502 *proc, enum address_mode mode) {
    proc->a &= get_operand(proc, mode);
    set_zn(proc);
}

void inst_BCS(struct m6502 *proc, enum address_mode mode) {
}

void inst_BVC(struct m6502 *proc, enum address_mode mode) {
}

void inst_INVALID(struct m6502 *proc, enum address_mode mode) {
    printf("invalid instruction at $%04x", proc->pc);
}

void inst_PLA(struct m6502 *proc, enum address_mode mode) {
}

//
// Cases:
// V C   Unsigned         Signed
// 0 0   64 +  80 = 144    64 +   80 =  144
// 1 0   80 +  80 = 160    80 +   80 =  -96
// 0 1  192 + 176 = 368   -64 +  -80 = -144
// 1 1  208 + 144 = 352   -48 + -112 =   96
//
void inst_ADC(struct m6502 *proc, enum address_mode mode) {
    uint16_t uresult = (uint8_t) proc->a + get_operand(proc, mode) + proc->c;
    proc->a = uresult & 0xff;
    set_zn(proc);
    proc->c = (uresult >> 8) & 1;

    // XXX probably broken
    int16_t sresult = (int8_t) proc->a + (int8_t) get_operand(proc, mode) + proc->c;
    proc->v = (sresult < -128 || sresult > 127);
}

void inst_CLC(struct m6502 *proc, enum address_mode mode) {
}

void inst_DEX(struct m6502 *proc, enum address_mode mode) {
}

void inst_CPX(struct m6502 *proc, enum address_mode mode) {
}

void inst_SED(struct m6502 *proc, enum address_mode mode) {
}

void inst_ORA(struct m6502 *proc, enum address_mode mode) {
    proc->a |= get_operand(proc, mode);
}

void inst_ROL(struct m6502 *proc, enum address_mode mode) {
}

void inst_STA(struct m6502 *proc, enum address_mode mode) {
}

void inst_CLD(struct m6502 *proc, enum address_mode mode) {
}

void inst_STY(struct m6502 *proc, enum address_mode mode) {
}

void inst_CLI(struct m6502 *proc, enum address_mode mode) {
}

void inst_STX(struct m6502 *proc, enum address_mode mode) {
}

void inst_TAY(struct m6502 *proc, enum address_mode mode) {
}

void inst_NOP(struct m6502 *proc, enum address_mode mode) {
}

void inst_INY(struct m6502 *proc, enum address_mode mode) {
}

void inst_LDY(struct m6502 *proc, enum address_mode mode) {
}

void inst_BRK(struct m6502 *proc, enum address_mode mode) {
    proc->running = 0;
}

void inst_PHA(struct m6502 *proc, enum address_mode mode) {
}

void inst_JMP(struct m6502 *proc, enum address_mode mode) {
}

void inst_BNE(struct m6502 *proc, enum address_mode mode) {
}

void inst_PHP(struct m6502 *proc, enum address_mode mode) {
}

void inst_RTI(struct m6502 *proc, enum address_mode mode) {
}

void inst_LDA(struct m6502 *proc, enum address_mode mode) {
    proc->a = get_operand(proc, mode);
    proc->z = proc->a == 0;
    proc->n = (proc->a & 0x80) != 0;
}

void inst_TXS(struct m6502 *proc, enum address_mode mode) {
}

void inst_TAX(struct m6502 *proc, enum address_mode mode) {
}

void inst_BMI(struct m6502 *proc, enum address_mode mode) {
}

void inst_TXA(struct m6502 *proc, enum address_mode mode) {
}

void inst_DEY(struct m6502 *proc, enum address_mode mode) {
}

void inst_TSX(struct m6502 *proc, enum address_mode mode) {
}

void inst_BCC(struct m6502 *proc, enum address_mode mode) {
}

void inst_BIT(struct m6502 *proc, enum address_mode mode) {
}

void inst_SEC(struct m6502 *proc, enum address_mode mode) {
}

void inst_SBC(struct m6502 *proc, enum address_mode mode) {
}

void inst_DEC(struct m6502 *proc, enum address_mode mode) {
}

void inst_LDX(struct m6502 *proc, enum address_mode mode) {
}

void inst_PLP(struct m6502 *proc, enum address_mode mode) {
}

void inst_INC(struct m6502 *proc, enum address_mode mode) {
}

void inst_CPY(struct m6502 *proc, enum address_mode mode) {
}

void inst_BPL(struct m6502 *proc, enum address_mode mode) {
}

void inst_BVS(struct m6502 *proc, enum address_mode mode) {
}

void inst_TYA(struct m6502 *proc, enum address_mode mode) {
}

void inst_SEI(struct m6502 *proc, enum address_mode mode) {
}

void inst_CLV(struct m6502 *proc, enum address_mode mode) {
}

void inst_INX(struct m6502 *proc, enum address_mode mode) {
}

void inst_BEQ(struct m6502 *proc, enum address_mode mode) {
}

void inst_JSR(struct m6502 *proc, enum address_mode mode) {
}

void inst_RTS(struct m6502 *proc, enum address_mode mode) {
}

void mainloop(struct m6502 *proc) {
    while (proc->running) {
        int opcode = read8(proc, proc->pc++);
        const struct instruction *inst = &INSTRUCTIONS[opcode];
        inst->func(proc, inst->mode);
    }
}

void init_proc(struct m6502 *proc) {
    proc->a = 0;
    proc->x = 0;
    proc->y = 0;
    proc->s = 0;
    proc->pc = 0; // XXX reset vector
    proc->n = 0;
    proc->v = 0;
    proc->b = 0;
    proc->d = 0;
    proc->i = 0;
    proc->z = 0;
    proc->c = 0;
    proc->memory = calloc(MEM_SIZE, 1);
    proc->running = 1;
}

void dump_regs(struct m6502 *proc) {
    printf("A %02x\n", proc->a & 0xff);
    printf("X %02x\n", proc->x & 0xff);
    printf("Y %02x\n", proc->y & 0xff);
    printf("S %02x\n", proc->s & 0xffff);
    printf("PC %04x\n", proc->pc);
    printf("      NVBDIZC\n");
    printf("Flags %d%d%d%d%d%d%d\n", proc->n, proc->v, proc->b, proc->d,
        proc->i, proc->z, proc->c);
}

void disassemble(uint16_t base_addr, uint8_t *memory, int length) {
    int offs = 0;
    while (offs < length) {
        int start_offs = offs;
        uint8_t opcode = memory[offs++];
        const struct instruction *inst = &INSTRUCTIONS[opcode];
        char operands[64];
        switch (inst->mode) {
            case ABSOLUTE:
                snprintf(operands, sizeof(operands), "$%04x", memory[offs] | (memory[offs + 1] << 8));
                offs += 2;
                break;
            case ABSOLUTE_X:
                snprintf(operands, sizeof(operands), "$%04x, X",
                    memory[offs] | (memory[offs + 1] << 8));
                offs += 2;
                break;
            case ABSOLUTE_Y:
                snprintf(operands, sizeof(operands), "$%04x, Y",
                    memory[offs] | (memory[offs + 1] << 8));
                offs += 2;
                break;
            case IMPLIED:
                strcpy(operands, "");
                break;
            case IND_ZERO_PAGE_X:
                snprintf(operands, sizeof(operands), "($%02x, X)",
                    memory[offs++]);
                break;
            case IND_ZERO_PAGE_Y:
                snprintf(operands, sizeof(operands), "($%02x), Y",
                    memory[offs++]);
                break;
            case IMMEDIATE:
                snprintf(operands, sizeof(operands), "#$%02x", memory[offs++]);
                break;
            case ZERO_PAGE_X:
                snprintf(operands, sizeof(operands), "$%02x, X",
                    memory[offs++]);
                break;
            case ZERO_PAGE:
                snprintf(operands, sizeof(operands), "$%02x", memory[offs++]);
                break;
        }

        char line[128];
        snprintf(line, sizeof(line), "%04x", base_addr + start_offs);
        for (int i = start_offs; i < offs; i++) {
            snprintf(line + strlen(line), sizeof(line) - strlen(line), " %02x", memory[i]);
        }

        while (strlen(line) < 20) {
            strcat(line, " ");
        }

        snprintf(line + strlen(line), sizeof(line) - strlen(line),
            " %s %s", inst->mnemonic, operands);
        printf("%s\n", line);
    }
}

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
    mainloop(&proc);
    dump_regs(&proc);

    return 0;
}
