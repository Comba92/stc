#ifndef STC_MAP_H
#define STC_MAP_H

#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "stc_list.h"
#include "stc_str.h"

size_t djb2(str s)
{
    size_t hash = 5381;

    for(int i=0; i<s.len; ++i)
        hash = ((hash << 5) + hash) + s.data[i]; /* hash * 33 + c */

    return hash;
}

#define map_define(type, name) map_entry_define(type, name, name##Pair)

#define map_entry_define(type, map_name, entry_name) \
    typedef struct { \
        str key; \
        type value; \
        bool occupied; \
    } entry_name; \
    list_define(entry_name, map_name); \
\
entry_name map_##entry_name##_new(str key, type value) { \
    return (entry_name) { \
        .key = key, \
        .value = value, \
        .occupied = true \
    }; \
} \
\
entry_name* map_##map_name##_get(map_name map, str key) { \
    for (int i=0; i<map.capacity; ++i) { \
        if (!map.data[i].occupied) continue; \
        if (str_cmp(map.data[i].key, key) == 0) { \
            return &map.data[i]; \
        } \
    } \
\
    return NULL; \
} \
\
void map_##map_name##_insert(map_name* map, entry_name keyvalue) { \
    size_t h = djb2(keyvalue.key)%map->capacity;  \
    int i; \
    for (i=0; \
        i<map->capacity && \
        map->data[h].occupied && \
        str_cmp(map->data[i].key, keyvalue.key) != 0;\
        ++i) \
    { \
        h = (h+1)%map->capacity; \
    } \
\
    if (map->data[h].occupied) { \
        if (str_cmp(map->data[h].key, keyvalue.key) != 0) { \
            printf("[MAP] table overflow, reallocating\n"); \
            map_name new; \
            map_init(new, map->capacity*2); \
            printf("[MAP] New map allocated with cap %lld\n", new.capacity); \
            for(int i=0; i<map->capacity; ++i) { \
                entry_name e = map->data[i]; \
                map_##map_name##_insert(&new, e); \
            } \
            map_##map_name##_insert(&new, keyvalue); \
            map->data = new.data; \
            map->len = new.len; \
            map->capacity = new.capacity; \
            return; \
        } \
        map->data[h].value = keyvalue.value; \
    } \
\
    map->data[h].occupied = true; \
    map->data[h].key = keyvalue.key; \
    map->data[h].value = keyvalue.value; \
    map->len += 1; \
} \

#define map_init(map, cap) \
    do { \
        (map).data = calloc((cap), sizeof(*(map).data)); \
        (map).len = 0; \
        (map).capacity = (cap); \
    } while(0)  \

#endif