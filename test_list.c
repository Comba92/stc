#include <stdio.h>
#include "stc_list.h"

list_define(char, CharList);

int main() {
    IntList nums = {0};
    IntList tmp = {0};

    for (int i=0; i<10; ++i) list_push(nums, i);
    for (int i=10; i<20; ++i) list_push(tmp, i);

    list_foreach(int, nums, n, { printf("%d\n", *n); })
    
    list_concat(int, nums, tmp);
    printf("Concat works:\n");
    list_foreach(int, nums, n, { printf("%d\n", *n); })
    
    int found;
    list_contains(int, nums, 9, found);
    printf("Contains found: %d\n", found);

    list_contains(int, nums, 100, found);
    printf("Contains found: %d\n", found);

    list_filter(int, nums, n, *n <= 5);
    printf("Filter works:\n");
    list_foreach(int, nums, n, { printf("%d\n", *n); })

    CharList chars = {0};
    list_map(int, nums, char, chars, n, (char)(*n + 'a'));
    printf("Map works:\n");
    list_foreach(char, chars, n, { printf("%c\n", *n); })

    Arena a = {0};
    IntList numsa = {0};
    printf("Arena push works:\n");
    for (int i=0; i<100; ++i) {
        printf("Len: %d, Cap: %d, Val: %d\n", numsa.len, numsa.capacity, i);
        arena_list_push(&a, numsa, i);
    }

    printf("%d %d\n", sizeof(uintptr_t), sizeof(Region) );
}