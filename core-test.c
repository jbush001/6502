#include <stdio.h>
#include <stdlib.h>
#include "6502-core.h"

#define TEST_EQ(x, y) { if (x != y) { printf("Test failed (line %d): "#x" != "#y"\n", __LINE__); exit(1); } }




int main() {
    TEST_EQ(0, 1);
}
