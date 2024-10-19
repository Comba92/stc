#include "stc_map.h"
#include <stdio.h>
#include <stdlib.h>

map_define(int, IntMap);

#define MAP_CAP 100

int main() {
    printf("Testing map...\n");
    IntMap map;
    map_init(map, MAP_CAP);
    printf("Map initialized\n");

    printf("This crashes? %d\n", map.data[0].occupied);
    Arena a = {0};

    for (int i=0; i<MAP_CAP; i++) {
        String n = int_to_string(&a, i);
        map_IntMap_insert(&map, map_IntMapPair_new(STR(n), i));
    }
    printf("Values inserted\n");

    for (int i=0; i<MAP_CAP; i++) {
        printf("Data in map -> %s: %d\n", zstr(map.data[i].key), map.data[i].value);
    }

    IntMapPair* res = map_IntMap_get(map, str_from("12"));
    if (!res) printf("Value not found\n");
    else printf("Value found: %d\n", res->value);

    map_IntMap_insert(&map, map_IntMapPair_new(str_from("realloc"), 69));
    printf("Rellocated has len: %d", map.len);

    arena_free(&a);
    zstrings_free();
}