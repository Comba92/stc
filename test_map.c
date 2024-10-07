#include "stc_map.h"
#include <stdio.h>
#include <stdlib.h>

map_define(int, IntMap);

#define MAP_CAP 20

int main() {
    printf("Testing map...\n");
    IntMap map;
    map_init(map, MAP_CAP);

    for (int i=0; i<MAP_CAP; i++) {
        char* buf = malloc(10);
        sprintf(buf, "%d", i);
        map_IntMap_insert(&map, (IntMapPair) {key: buf, value: i, occupied: true});
    }

    for (int i=0; i<MAP_CAP; i++) {
        printf("Data in map -> %s: %d\n", map.data[i].key, map.data[i].value);
    }

    IntMapPair* res = map_IntMap_get(map, "12");
    if (!res) printf("Value not found\n");
    else printf("Value found: %d\n", res->value);
}