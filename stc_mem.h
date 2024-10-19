#ifndef STC_MEM_H
#define STC_MEM_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

#define DEFAULT_ALIGNMENT 64
static size_t align_forward(size_t ptr) {
    // If this modulo == 0, the address is aligned
    // Same as (p % a) but faster as 'a' is a power of two
    // size_t modulo = ptr & (DEFAULT_ALIGNMENT - 1);
    // if (modulo != 0) {
        // If modulo is != 0, the addres is not aligned
        // Push to the next aligned value
        // ptr += DEFAULT_ALIGNMENT - modulo;
    // }

    size_t aligned = (ptr + (DEFAULT_ALIGNMENT - 1)) & -DEFAULT_ALIGNMENT;
    // size_t padding = -(ptr) & (DEFAULT_ALIGNMENT - 1);

    return aligned;
}

/* Allocs a region of size cap, aligned to memory.
 * @param cap: size to alloc
*/
static Region* region_new(size_t region_cap) {
    size_t bytes = sizeof(Region) + region_cap;
    bytes = align_forward(bytes);
    Region* r = (Region*) malloc(bytes);
    
    assert("malloc failed to allocate region" && r != NULL);

    printf("[ARENA]: allocated region of size %ld bytes (total of %ld bytes with header and alignment)\n", region_cap, bytes);

    r->allocated = 0;
    r->capacity = region_cap;
    r->next = 0;
    return r;
}

typedef struct Arena {
    Region* head;
    Region* curr;
} Arena;

#define DEFAULT_REGION_CAP 2096
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

    while (a->curr->allocated + bytes_to_alloc > a->curr->capacity
    && a->curr->next != NULL) {
        a->curr = a->curr->next;
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

/* Allocs some data into the arena by copying it.
 * @param a: the arena
 * @param data: the data to copy
 * @param bytes: size to alloc
*/
void* arena_copy(Arena* a, void* data, size_t size) {
    return memcpy(arena_alloc(a, size), data, size);
}


/* Reallocs some data into the arena, copying it, and giving back the reallocated data pointer.
 * @param a: the arena, might be initialized
 * @param old_data: the data to realloc
 * @param: old_size: the old size of the data to realloc
 * @param: new_size: the new size of the data to realloc
 * @param bytes: size to alloc
*/
void* arena_realloc(Arena* a, void* old_data, size_t old_size, size_t new_size) {
    if (old_size >= new_size) return old_data;
    
    void* new_data = arena_alloc(a, new_size);
    
    for (int i=0; i<old_size; ++i) {
        ((char*)new_data)[i] = ((char*)old_data)[i];
    }

    return new_data;
}

/* Deletes all data from the arena, without freeing the regions.
 * @param a: the arena
 */
void arena_reset(Arena* a)
{
    for (Region *r = a->head; r != NULL; r = r->next) {
        r->allocated = 0;
    }

    a->curr = a->head;
}

/* Completely frees the arena, freeing the regions.
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



typedef struct Chunk Chunk;
struct Chunk {
    Chunk* next;
};

typedef struct Pool {
    Arena arena;
    size_t chunk_size;
    Chunk* curr;
} Pool;

#define DEFAULT_POOL_CHUNKS 128
static void pool_build_freelist(Pool* p) {
    for (int i=0; i<DEFAULT_POOL_CHUNKS; ++i) {
        Chunk* next = arena_alloc(&p->arena, p->chunk_size);
        next->next = p->curr;
        p->curr = next;
    }
}

Pool pool_new(size_t chunk_bytes) {
    Pool p = {0};
    p.chunk_size = sizeof(Chunk) + chunk_bytes;
    p.arena.curr = p.arena.head = region_new(p.chunk_size * DEFAULT_POOL_CHUNKS);

    pool_build_freelist(&p);

    return p;
}

void* pool_alloc(Pool* p) {
    if (p->curr == NULL) {
        if (p->arena.curr->next == NULL) {
            p->arena.curr->next = region_new(p->chunk_size * DEFAULT_POOL_CHUNKS);
        }

        p->arena.curr = p->arena.curr->next;
        pool_build_freelist(p);
    }

    Chunk* chunk = p->curr;
    p->curr = p->curr->next;
    return (void*) (chunk + 1);
}

void pool_free(Pool* p, void* ptr) {
    Region* curr = p->arena.head;
    while (curr != NULL 
    && !(curr->data <= (char*)ptr && (char*)ptr < curr->data + curr->allocated)) {
        curr = curr->next;
    }
    assert("pointer not in pool" && curr != NULL);

    Chunk* chunk = ptr;
    chunk->next = p->curr;
    p->curr = chunk;
}

void pool_reset(Pool* p) {
    arena_reset(&p->arena);
    pool_build_freelist(p);
}

#endif