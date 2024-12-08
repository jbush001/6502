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
    int running;
};

#endif