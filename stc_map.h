#ifndef STC_MAP_H
#define STC_MAP_H

#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "stc_list.h"

size_t djb2(char* str)
{
    size_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


#define map_define(type, name) \
    typedef struct { \
        char* key; \
        type value; \
        bool occupied; \
    } name##Pair; \
    list_define(name##Pair, name); \
\
name##Pair* map_##name##_get(name map, char* key) { \
    for (int i=0; i<map.capacity; ++i) { \
        if (!map.data[i].occupied) continue; \
        if (strcmp(map.data[i].key, key) == 0) { \
            return &map.data[i]; \
        } \
    } \
\
    return NULL; \
} \
\
void map_##name##_insert(name* map, name##Pair entry) { \
    size_t h = djb2(entry.key)%map->capacity;  \
\
    for (int i=0; \
        i<map->capacity \
        && map->data[h].occupied \
        && strcmp(map->data[i].key, entry.key) != 0; \
        ++i) \
    { \
        h = (h+1)%map->capacity; \
    } \
\
    if (map->data[h].occupied) { \
        if (strcmp(map->data[h].key, entry.key) != 0) { \
            assert("table overflow" && false); \
        } \
        map->data[h].value = entry.value; \
    } \
\
    map->data[h].occupied = true; \
    map->data[h].key = entry.key; \
    map->data[h].value = entry.value; \
    map->len += 1; \
} \

#define map_init(map, cap) \
    do { \
        (map).data = malloc(sizeof(*(map).data)*cap); \
        memset((map).data, 0, sizeof(*(map).data)*cap); \
        (map).len = 0; \
        (map).capacity = (cap); \
    } while(0)  \

#endif