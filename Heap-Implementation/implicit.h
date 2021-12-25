#ifndef _IMPLICIT_H_
#define _IMPLICIT_H_

#include <stdint.h>
#include <stdalign.h>

/*
 * Search algorithm used for the heap.
 */
typedef enum { HEAP_FIRSTFIT, HEAP_NEXTFIT, HEAP_BESTFIT } search_alg_t;

/*
 * Maximum amount of empty space in a block.
 */
#define MAX_UNUSED_BYTES 128

typedef uint32_t block_size_t;
typedef uint64_t payload_align_t;

#define HEADER_SIZE (sizeof(block_size_t)) // same as footer size
#define PAYLOAD_ALIGN (alignof(payload_align_t))

/*
 * Struct used to represent the heap.
 */
typedef struct heap {
    search_alg_t search_alg; /* Search algorithm. */
    intptr_t size;           /* Size of the heap in bytes. */
    void *next;              /* Next block to try (for next fit only). */
    void *start;             /* Start address of the heap area. */
} heap;

/*
 * Create a heap that is "size" bytes large.
 */
heap *heap_create(intptr_t size, search_alg_t search_alg);

/*
 * Print the structure of the heap to the screen.
 */
void heap_print(heap *h);

/*
 * Determine the average size of a free block.
 */
block_size_t heap_find_avg_free_block_size(heap *h);

/*
 * Free a block on the heap h.
 */
void heap_free(heap *h, void *payload);

/*
 * Our implementation of malloc.
 */
void *heap_malloc(heap *h, block_size_t size);

#endif
