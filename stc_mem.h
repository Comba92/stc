#ifndef STC_MEM_H
#define STC_MEM_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Region for Arena allocation.
 * @param next: pointer to the next region of the arena 
 * @param allocated: how many bytes of the region are used
 * @param capacity: size in bytes of the region
 * @param data: the allocatet data
 */
typedef struct Region Region;
struct Region {
    Region* next;
    size_t allocated;
    size_t capacity;
    char data[];
};

/* Allocs a region of size cap.
 * @param cap: size to alloc
*/
static Region* region_new(size_t region_cap) {
    size_t bytes = sizeof(Region) + region_cap*sizeof(char);
    Region* r = (Region*) malloc(bytes);
    assert(r != NULL);

    printf("[ARENA]: allocated region of size %ld bytes (total of %ld bytes with header)\n", region_cap, bytes);
    r->allocated = 0;
    r->capacity = region_cap;
    r->next = 0;
    return r;
}

typedef struct Arena {
    Region* head;
    Region* curr;
} Arena;

#define DEFAULT_REGION_CAP 1024
/* Allocs the requested bytes on the arena.
 * If the arena is uninitialized or has been freed, it will be initialized.
 * If the current region is not big enough, a new one will be allocated. 
 * @param a: the arena, might be initialized
 * @param bytes: size to alloc
*/
void* arena_alloc(Arena* a, size_t bytes_to_alloc) {
    size_t region_cap = bytes_to_alloc > DEFAULT_REGION_CAP ? bytes_to_alloc : DEFAULT_REGION_CAP;
    
    // uninitialized/freed arena
    if (a->curr == NULL) {
        a->head = a->curr = region_new(region_cap);
    }

    // arena too small, allocate new region
    if (a->curr->allocated + bytes_to_alloc > a->curr->capacity) {
        a->curr->next = region_new(region_cap);
        a->curr = a->curr->next;
    }

    void* res = &a->curr->data[a->curr->allocated];
    a->curr->allocated += bytes_to_alloc;
    return res;
}

void* arena_realloc(Arena* a, void* old_data, size_t old_size, size_t new_size) {
    if (old_size <= new_size) return old_data;
    void* new_data = arena_alloc(a, new_size);
    
    for (int i=0; i<old_size; ++i) {
        ((char*)new_data)[i] = ((char*)old_data)[i];
    }

    return new_data;
}

/* Deletes all data from the arena, without freeing the region.
 * @param a: the arena
 */
void arena_reset(Arena* a)
{
    for (Region *r = a->head; r != NULL; r = r->next) {
        r->allocated = 0;
    }

    a->curr = a->head;
}

/* Completely frees the arena.
 * @param a: the arena
*/
void arena_free(Arena* a) {
    Region* r = a->head;
    assert(r != NULL);

    while (r != NULL) {
        Region* to_delete = r;
        r = r->next;
        free(to_delete);
    }

    a->head = a->curr = NULL;
}

#endif