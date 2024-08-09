#ifndef STC_H
#define STC_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define _LIST_BASE_CAPACITY 32

#define list_define(type, name)    \
    typedef struct {                \
        type* data;                 \
        size_t len;               \
        size_t capacity;            \
    } name;                         \

#define slice_define(type, name)    \
    typedef struct {                \
        type* data;                 \
        size_t len;               \
    } name;                         \


/*  Pushes the relative value type to the end of the typed list.
 *  Will be reallocated if capacity is full.
 *  @param list: the typed list to push on.
 *  @param value: the value to push. Must be of the same type of the typed list. 
 */
#define list_push(list, value)                                          \
    do {                                                                \
        if ( (list).len == 0 ) {                                        \
            (list).capacity = _LIST_BASE_CAPACITY;                      \
            (list).data = malloc(sizeof(value) * (list).capacity);      \
        }                                                               \
        if ( (list).len >= (list).capacity ) {                          \
            (list).capacity *= 2;                                       \
            (list).data = realloc((list).data, sizeof(value) * (list).capacity);      \
        }                                                               \
        (list).data[(list).len] = (value);                              \
        (list).len += 1;                                                \
    } while(0)
    


#define list_pop(list) ( (list).len > 0 ? (list).data[--(list).len] : 0 )                                                        \

// Frees the typed list, and sets its length to 0.
#define list_free(list)           \
    do {                            \
        if ((list).len != 0) {   \
            free((list).data);     \
            (list).len = 0;      \
        }                           \
    } while (0)                     \

// NOTE TO SELF: always pass an already computed list as a parameter, or else it will be recomputed each iter
/*  Generic foreach for typed lists. 
 *  @param type: the type of the list.
 *  @param list: the typed list.
 *  @param elem: a symbol, the name of the current value. It will get a pointer to the value.
 *  @param body: a statement to execute for each iteration.
 */
#define foreach(type, list, elem, body)    \
    for (size_t i = 0;                      \
         i < (list).len;                 \
         ++i)                               \
    {                                       \
        type* elem = &(list).data[i];      \
        body;                               \
    }                                       \

// Concats the list_from to list_to in-place
#define list_concat(type, list_to, list_from)    \
    do {                                            \
        foreach(type, (list_from), it, { list_push((list_to), (*it)); }); \
    } while(0)                                      \

// @param found: an already defined bool value must be passed here, which will hold the result. 
#define list_contains(type, list, target, found) \
    do {                                           \
        found = false;                             \
        foreach(type, (list), elem, { if (*elem == (target)) { found = true; break; } }); \
    } while(0)                                    \

/*  Filters the list in-place
 *  @param elem: a symbol, the name of the current value. It will get a pointer to the value.
 *  @param condition: a bool expression, should be used *elem in it.
 */
#define list_filter(type, list, elem, condition)  \
    do {                                            \
        int last_found = 0;                         \
        foreach(type, (list), elem, { if(condition) { (list).data[last_found] = *elem; last_found += 1; } }) \
        (list).len = last_found;                 \
    } while(0)                                      \


// ------------------------------------------------------ //
// -----------------------Hashmap------------------------ //
// ------------------------------------------------------ //

#define map_define(type, name) \
    typedef struct {                \
        String key;                 \
        type value;                 \
        bool occupied;              \
    } name##Pair;                   \
    list_define(name##Pair, name);   \

#define map_init(map, cap) \
    do { \
        (map).data = malloc(sizeof(*(map).data) * cap); \
        memset((map).data, 0, sizeof(*(map).data)*cap); \
        (map).len = 0; \
        (map).capacity = (cap); \
    } while(0)  \


// string hashing alogorithm
uint32_t djb2(uint8_t *buf, size_t buf_size)
{
    uint32_t hash = 5381;

    for (size_t i = 0; i < buf_size; ++i) {
        hash = ((hash << 5) + hash) + (uint32_t)buf[i]; /* hash * 33 + c */
    }

    return hash;
}

#endif