#include "stc_mem.h"

typedef struct Point {
    double x, y, z;
} Point;

int main() {
    Arena a = {0};

    for (int i=0; i<8; i++) {
        arena_alloc(&a, 500);
        arena_alloc(&a, 512);
        arena_alloc(&a, 600);
    }

    arena_reset(&a);
    printf("Arena cleared.\n");

    for (int i=0; i<8; i++) {
        arena_alloc(&a, 500);
        arena_alloc(&a, 512);
        arena_alloc(&a, 600);
    }

    arena_free(&a);

    Point* buf[200];
    Pool p = pool_new(sizeof(Point));
    for (int i=0; i<200; i++) {
        pool_alloc(&p);
    }
    pool_reset(&p);
    printf("Pool reset\n");
    for (int i=0; i<200; i++) {
        buf[i] = pool_alloc(&p);
    }

    for(int i=0; i<100; i++) {
        pool_free(&p, buf[i]);
    }

    printf("Free tried...\n");
    for (int i=0; i<500; i++) {
        pool_alloc(&p);
    }

    pool_alloc(&p);
    pool_free(&p, buf[100]);

    printf("error here...\n");
    void* test = malloc(10);
    pool_free(&p, test);
}
