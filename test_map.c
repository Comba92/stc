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
    Arena a = {0};

    for (int i=0; i<MAP_CAP*2; i++) {
        String n = int_to_string(&a, i);
        map_IntMap_insert(&map, STR(n), i);
    }
    printf("Values inserted\n");

    map_foreach(IntMap, map, entry, {printf("Data in map -> %s: %d\n", zstr(entry->key), entry->value);});

    IntMapEntry* res = map_IntMap_get(map, str_from("12"));
    if (!res) printf("Value not found\n");
    else printf("Value found: %d\n", res->value);

    map_IntMap_insert(&map, str_from("realloc"), 69);
    printf("Rellocated has len: %d", map.len);

    map_free(map);
    arena_free(&a);
    zstrings_free();
}