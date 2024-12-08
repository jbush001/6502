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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "6502-core.h"
#include "instructions.h"

uint8_t read_mem_u8(struct m6502 *proc, uint16_t addr) {
    return proc->memory[addr];
}

void write_mem_u8(struct m6502 *proc, uint16_t addr, uint8_t val) {
    proc->memory[addr] = val;
}

uint16_t read_mem_u16(struct m6502 *proc, uint16_t addr) {
    return proc->memory[addr] | (proc->memory[addr + 1] << 8);
}

uint16_t get_operand_addr(struct m6502 *proc, enum address_mode mode) {
    switch (mode) {
        case IND_ZERO_PAGE_X: { // ($hh, X)
            unsigned short addr = read_mem_u8(proc, proc->pc++) + proc->x;
            return read_mem_u16(proc, addr);
        }

        case ZERO_PAGE: // $hh
            return read_mem_u8(proc, proc->pc++);

        case ABSOLUTE: { // $hhhh
            unsigned short addr = read_mem_u16(proc, proc->pc);
            proc->pc += 2;
            return addr;
        }

        case IND_ZERO_PAGE_Y: // ($hh), y
            return read_mem_u16(proc, read_mem_u8(proc, proc->pc++)) + proc->y;

        case ZERO_PAGE_X: // $hh, X
            return read_mem_u8(proc, proc->pc++) + proc->x;

        case ABSOLUTE_X: { // $hhhh, X
            unsigned short addr = read_mem_u16(proc, proc->pc);
            proc->pc += 2;
            return addr + proc->x;
        }

        case ABSOLUTE_Y: { // $hhhh, Y
            unsigned short addr = read_mem_u16(proc, proc->pc);
            proc->pc += 2;
            return addr + proc->y;
        }

        case INDIRECT:
        case IMPLIED:
        default:
            assert(0);
            return 0;
    }
}

uint8_t get_operand_value(struct m6502 *proc, enum address_mode mode) {
    if (mode == IMPLIED) {
        return proc->a;
    }

    if (mode == IMMEDIATE) {
        return read_mem_u8(proc, proc->pc++);
    }

    return read_mem_u8(proc, get_operand_addr(proc, mode));
}

void set_nz_flags(struct m6502 *proc, uint8_t value) {
    proc->n = (value >> 7) & 1;
    proc->z = (value & 0xff) == 0;
}

void inst_INVALID(struct m6502 *proc, enum address_mode mode) {
    printf("invalid instruction at $%04x\n", proc->pc - 1);
    proc->halt = 1;
}

void inst_BRK(struct m6502 *proc, enum address_mode mode) {
    proc->halt = 1;
}

void inst_NOP(struct m6502 *proc, enum address_mode mode) {
}

//
// Arithmetic
//
#define UNARY_OP(__op__) \
   if (mode == IMPLIED) { \
        uint8_t old_val = proc->a; \
        uint8_t new_val = __op__; \
        proc->a = new_val; \
        set_nz_flags(proc, new_val); \
    } else { \
        uint16_t addr = get_operand_addr(proc, mode); \
        uint8_t old_val = read_mem_u8(proc, addr); \
        uint8_t new_val = __op__; \
        set_nz_flags(proc, new_val); \
        write_mem_u8(proc, addr, new_val); \
    }

void inst_LSR(struct m6502 *proc, enum address_mode mode) {
    UNARY_OP((old_val >> 1); proc->c = (old_val >> 7) & 1);
}

void inst_ASL(struct m6502 *proc, enum address_mode mode) {
    UNARY_OP((old_val << 1); proc->c = (old_val >> 7) & 1);
}

void inst_ROL(struct m6502 *proc, enum address_mode mode) {
    UNARY_OP((old_val << 1) | proc->c; proc->c = (old_val >> 7) & 1);
}

void inst_ROR(struct m6502 *proc, enum address_mode mode) {
    UNARY_OP((old_val >> 1) | (proc->c << 7); proc->c = old_val & 1);
}

void inst_EOR(struct m6502 *proc, enum address_mode mode) {
    proc->a ^= get_operand_value(proc, mode);
    set_nz_flags(proc, proc->a);
}

void inst_ORA(struct m6502 *proc, enum address_mode mode) {
    proc->a |= get_operand_value(proc, mode);
    set_nz_flags(proc, proc->a);
}

void inst_AND(struct m6502 *proc, enum address_mode mode) {
    proc->a &= get_operand_value(proc, mode);
    set_nz_flags(proc, proc->a);
}

void inst_BIT(struct m6502 *proc, enum address_mode mode) {
    assert(0); // Not implemented
}

uint8_t negate(uint8_t val) {
    return (val ^ 0xff) + 1;
}

uint8_t add(struct m6502 *proc, uint8_t op1, uint8_t op2) {
    uint16_t uresult = op1 + op2 + proc->c;
    set_nz_flags(proc, uresult);

    // Carry occurs when an unsigned value does not fit in the
    // register. e.g. 208 + 144 = 352
    proc->c = (uresult >> 8) & 1;

    // Overflow indicates a signed arithmetic operation has wrapped
    // around, inverting the sign. It can only occur when the signs
    // of the two operands are the same and the result has a different
    // sign. e.g. in 8 bit Two's complement:
    // -48 + -112 = 96 and 80 + 80 = -96.
    int sign1 = op1 >> 7;
    int sign2 = op2 >> 7;
    int result_sign = (uresult >> 7) & 1;
    proc->v = sign1 == sign2 && sign1 != result_sign;

    return uresult & 0xff;
}

void inst_CMP(struct m6502 *proc, enum address_mode mode) {
    proc->c = 0;
    add(proc, proc->a, negate(get_operand_value(proc, mode)));
}

void inst_CPX(struct m6502 *proc, enum address_mode mode) {
    proc->c = 0;
    add(proc, proc->x, negate(get_operand_value(proc, mode)));
}

void inst_CPY(struct m6502 *proc, enum address_mode mode) {
    proc->c = 0;
    add(proc, proc->y, negate(get_operand_value(proc, mode)));
}

void inst_ADC(struct m6502 *proc, enum address_mode mode) {
    proc->a = add(proc, proc->a, get_operand_value(proc, mode));
}

void inst_SBC(struct m6502 *proc, enum address_mode mode) {
    proc->a = add(proc, proc->a, negate(get_operand_value(proc, mode)));
}

void inst_INC(struct m6502 *proc, enum address_mode mode) {
    uint16_t addr = get_operand_addr(proc, mode);
    uint8_t new_val = read_mem_u8(proc, addr) + 1;
    set_nz_flags(proc, new_val);
    write_mem_u8(proc, addr, new_val);
}

void inst_DEC(struct m6502 *proc, enum address_mode mode) {
    uint16_t addr = get_operand_addr(proc, mode);
    uint8_t new_val = read_mem_u8(proc, addr) - 1;
    set_nz_flags(proc, new_val);
    write_mem_u8(proc, addr, new_val);
}

void inst_INX(struct m6502 *proc, enum address_mode mode) {
    set_nz_flags(proc, ++proc->x);
}

void inst_DEX(struct m6502 *proc, enum address_mode mode) {
    set_nz_flags(proc, --proc->x);
}

void inst_INY(struct m6502 *proc, enum address_mode mode) {
    set_nz_flags(proc, ++proc->y);
}

void inst_DEY(struct m6502 *proc, enum address_mode mode) {
    set_nz_flags(proc, --proc->y);
}

//
// Register moves
//
void inst_LDA(struct m6502 *proc, enum address_mode mode) {
    proc->a = get_operand_value(proc, mode);
    set_nz_flags(proc, proc->a);
}

void inst_STA(struct m6502 *proc, enum address_mode mode) {
    write_mem_u8(proc, get_operand_addr(proc, mode), proc->a);
}

void inst_LDX(struct m6502 *proc, enum address_mode mode) {
    proc->x = get_operand_value(proc, mode);
    set_nz_flags(proc, proc->a);
}

void inst_STX(struct m6502 *proc, enum address_mode mode) {
    write_mem_u8(proc, get_operand_addr(proc, mode), proc->x);
}

void inst_LDY(struct m6502 *proc, enum address_mode mode) {
    proc->y = get_operand_value(proc, mode);
    set_nz_flags(proc, proc->y);
}

void inst_STY(struct m6502 *proc, enum address_mode mode) {
    write_mem_u8(proc, get_operand_addr(proc, mode), proc->y);
}

void inst_TXS(struct m6502 *proc, enum address_mode mode) {
    proc->s = proc->x;
    set_nz_flags(proc, proc->s);
}

void inst_TSX(struct m6502 *proc, enum address_mode mode) {
    proc->x = proc->s;
    set_nz_flags(proc, proc->x);
}

void inst_TAX(struct m6502 *proc, enum address_mode mode) {
    proc->x = proc->a;
    set_nz_flags(proc, proc->x);
}

void inst_TXA(struct m6502 *proc, enum address_mode mode) {
    proc->a = proc->x;
    set_nz_flags(proc, proc->a);
}

void inst_TAY(struct m6502 *proc, enum address_mode mode) {
    proc->y = proc->a;
    set_nz_flags(proc, proc->y);
}

void inst_TYA(struct m6502 *proc, enum address_mode mode) {
    proc->a = proc->y;
    set_nz_flags(proc, proc->a);
}

void inst_PHA(struct m6502 *proc, enum address_mode mode) {
    write_mem_u8(proc, proc->s-- + 0x100, proc->a);
}

void inst_PLA(struct m6502 *proc, enum address_mode mode) {
    proc->a = read_mem_u8(proc, ++proc->s + 0x100);
    set_nz_flags(proc, proc->a);
}

void inst_PHP(struct m6502 *proc, enum address_mode mode) {
    // XXX not implemented: push flags on the stack
    assert(0);
}

void inst_PLP(struct m6502 *proc, enum address_mode mode) {
    // XXX not implemented, pop flags from the stack.
    assert(0);
}

//
// Setting/clearing flags
//
void inst_SEC(struct m6502 *proc, enum address_mode mode) {
    proc->c = 1;
}

void inst_CLC(struct m6502 *proc, enum address_mode mode) {
    proc->c = 0;
}

void inst_SED(struct m6502 *proc, enum address_mode mode) {
    proc->d = 1;
}

void inst_CLD(struct m6502 *proc, enum address_mode mode) {
    proc->d = 0;
}

void inst_SEI(struct m6502 *proc, enum address_mode mode) {
    proc->i = 1;
}

void inst_CLI(struct m6502 *proc, enum address_mode mode) {
    proc->i = 0;
}

void inst_CLV(struct m6502 *proc, enum address_mode mode) {
    proc->v = 0;
}

//
// Branch
//
void inst_BCS(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (proc->c) {
        proc->pc += offset;
    }
}

void inst_BCC(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (!proc->c) {
        proc->pc += offset;
    }
}

void inst_BVS(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (proc->v) {
        proc->pc += offset;
    }
}

void inst_BVC(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (!proc->v) {
        proc->pc += offset;
    }
}

void inst_BMI(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (proc->n) {
        proc->pc += offset;
    }
}

void inst_BPL(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (!proc->n) {
        proc->pc += offset;
    }
}

void inst_BEQ(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (proc->z) {
        proc->pc += offset;
    }
}

void inst_BNE(struct m6502 *proc, enum address_mode mode) {
    int8_t offset = read_mem_u8(proc, proc->pc++);
    if (!proc->z) {
        proc->pc += offset;
    }
}

void inst_JMP(struct m6502 *proc, enum address_mode mode) {
    if (mode == ABSOLUTE) {
        proc->pc = read_mem_u16(proc, proc->pc);
    } else {
        // Indirect
        proc->pc = read_mem_u16(proc, read_mem_u16(proc, proc->pc));
    }
}

void inst_JSR(struct m6502 *proc, enum address_mode mode) {
    uint16_t target = read_mem_u16(proc, proc->pc);
    write_mem_u8(proc, proc->s-- + 0x100, proc->pc >> 8);
    write_mem_u8(proc, proc->s-- + 0x100, proc->pc & 0xff);
    proc->pc = target;
}

void inst_RTS(struct m6502 *proc, enum address_mode mode) {
    uint16_t ra = read_mem_u8(proc, ++proc->s + 0x100);
    ra = ra | (read_mem_u8(proc, ++proc->s + 0x100) << 8);
    proc->pc = ra;
}

void inst_RTI(struct m6502 *proc, enum address_mode mode) {
    assert(0); // Not implemented
}

void run_emulator(struct m6502 *proc) {
    proc->halt = 0;
    while (!proc->halt) {
        int opcode = read_mem_u8(proc, proc->pc++);
        const struct instruction *inst = &INSTRUCTIONS[opcode];
        inst->func(proc, inst->mode);
    }
}

void init_proc(struct m6502 *proc) {
    proc->a = 0;
    proc->x = 0;
    proc->y = 0;
    proc->s = 0xff;
    proc->pc = 0; // XXX reset vector
    proc->n = 0;
    proc->v = 0;
    proc->b = 0;
    proc->d = 0;
    proc->i = 0;
    proc->z = 0;
    proc->c = 0;
    proc->memory = calloc(MEM_SIZE, 1);
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
            case INDIRECT:
                snprintf(operands, sizeof(operands), "($%04x)",
                    memory[offs] | (memory[offs + 1] << 8));
                offs += 2;
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

