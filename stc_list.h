#ifndef STC_LIST_H
#define STC_LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "stc_mem.h"

#define list_define(type, name)     \
    typedef struct {                \
        type* data;                 \
        size_t len;                 \
        size_t capacity;            \
    } name;                         \

#define slice_define(type, name)    \
    typedef struct {                \
        type* data;                 \
        size_t len;                 \
    } name;                         \


list_define(int, IntList);
list_define(float, FloatList);
list_define(double, DoubleList);

/*  Pushes the relative value type to the end of the typed list.
 *  Will be reallocated if capacity is full.
 *  @param list: the typed list to push on.
 *  @param value: the value to push. Must be of the same type of the typed list. 
 */

#define DEFAULT_LIST_CAP 32
#define list_push(list, value)                                          \
    do {                                                                \
        if ( (list).len == 0 ) {                                        \
            (list).capacity = DEFAULT_LIST_CAP;                         \
            (list).data = malloc(sizeof(value) * (list).capacity);      \
        }                                                               \
        if ( (list).len >= (list).capacity ) {                          \
            (list).capacity *= 2;                                       \
            (list).data = realloc((list).data, sizeof(value) * (list).capacity);      \
        }                                                               \
        (list).data[(list).len] = (value);                              \
        (list).len += 1;                                                \
    } while(0)

#define DEFAULT_ARENA_LIST_CAP 128
#define arena_list_push(a, list, value)                                    \
    do {                                                                \
        if ( (list).len == 0 ) {                                        \
            (list).capacity = DEFAULT_ARENA_LIST_CAP;                   \
            (list).data = arena_alloc((a), sizeof(value) * (list).capacity); \
        }                                                               \
        if ( (list).len >= (list).capacity ) {                          \
            (list).capacity *= 2;                                       \
            (list).data = arena_realloc(                                \
                (a), (list).data,                                       \
                sizeof(value) * (list).len,                             \
                sizeof(value) * (list).capacity                         \
            );                                                          \
        }                                                               \
        (list).data[(list).len] = (value);                              \
        (list).len += 1;                                                \
    } while(0)

#define list_pop(list) ( (list).len > 0 ? (list).data[--(list).len] : 0 )                                                        \

// Frees the typed list, and sets its length to 0.
#define list_free(list)             \
    do {                            \
        if ((list).len != 0) {      \
            free((list).data);      \
            (list).len = 0;         \
        }                           \
    } while (0)                     \

// NOTE TO SELF: always pass an already computed list as a parameter, or else it will be recomputed each iter
/*  Generic foreach for typed lists. 
 *  @param type: the type of the list.
 *  @param list: the typed list.
 *  @param elem: a symbol, the name of the current value. It will get a pointer to the value.
 *  @param body: a statement to execute for each iteration.
 */
#define foreach(type, list, elem, body)     \
    for (size_t i = 0;                      \
         i < (list).len;                    \
         ++i)                               \
    {                                       \
        type* elem = &(list).data[i];       \
        body;                               \
    }                                       \

// Concats list_from to list_to in-place
#define list_concat(type, list_to, list_from)                               \
    do {                                                                    \
        foreach(type, (list_from), it, { list_push((list_to), (*it)); });   \
    } while(0)                                                              \

#define arena_list_concat(a, type, list_to, list_from)                                  \
    do {                                                                                \
        foreach(type, (list_from), it, { arena_list_push((a), (list_to), (*it)); });    \
    } while(0)    

#define list_find(type, list, target, idx)                                              \
do {                                                                                    \
        idx = -1;                                                                       \
        foreach(type, (list), elem, { if (*elem == (target)) { idx = i; break; } });    \
} while(0)                                                                              \

// @param found: an already defined bool value must be passed here, which will hold the result. 
#define list_contains(type, list, target, found)                                          \
    do {                                                                                  \
        found = false;                                                                    \
        foreach(type, (list), elem, { if (*elem == (target)) { found = true; break; } }); \
    } while(0)                                                                            \

/*  Filters the list in-place
 *  @param elem: a symbol, the name of the current value. It will get a pointer to the value.
 *  @param condition: a bool expression, should be used *elem in it.
 */
#define list_filter(type, list, elem, condition)                                                                \
    do {                                                                                                        \
        int last_found = 0;                                                                                     \
        foreach(type, (list), elem, { if(condition) { (list).data[last_found] = *elem; last_found += 1; } })    \
        (list).len = last_found;                                                                                \
    } while(0)                                                                                                  \


#define list_map(old_type, old_list, new_type, new_list, elem, mapping) \
    do {                                                                \
        foreach(old_type, (old_list), elem, {                           \
            new_type new_elem = (mapping);                              \
            list_push(new_list, new_elem);                              \
        })                                                              \
    } while(0)                                                          \

#define arena_list_map(a, old_type, old_list, new_type, new_list, elem, mapping) \
    do {                                                                \
        foreach(old_type, (old_list), elem, {                           \
            new_type new_elem = (mapping);                              \
            arena_list_push((a), (new_list), (new_elem));               \
        })                                                              \
    } while(0)                                                          \

#endif