#include "stc_mem.h"

int main() {
    Arena a = {0};

    for (int i=0; i<8; i++) {
        arena_alloc(&a, 512);
    }

    arena_free(&a);
}