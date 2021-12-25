#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>

#include "implicit.h"

/*
 * Determine whether or not a block is in use.
 */
static inline int block_is_in_use(void *block_start) {
  return 1 & *((block_size_t *) block_start);
}

/*
 * Return the size of a block.
 */
static inline block_size_t get_block_size(void *block_start) {
  return -HEADER_SIZE & *((block_size_t *) block_start);
}

/*
 * Return the size of the payload of a block.
 */
static inline block_size_t get_payload_size(void *block_start) {
  return get_block_size(block_start) - HEADER_SIZE * 2;
}

/*
 * Find the start of the block, given a pointer to the payload.
 */
static inline void *get_block_start(void *payload) {
  return payload - HEADER_SIZE;
}

/*
 * Find the payload, given a pointer to the start of the block.
 */
static inline void *get_payload(char *block_start) {
  return block_start + HEADER_SIZE;
}

/*
 * Set the size of a block, and whether or not it is in use. Remember
 * each block has two copies of the header (one at each end).
 */
static inline void set_block_header(void *block_start, block_size_t block_size, int in_use) {
  
  block_size_t header_value = block_size | in_use;
  *((block_size_t *) block_start) = header_value;
  *((block_size_t *) (get_payload(block_start) +
		      get_payload_size(block_start))) = header_value;
}

/*
 * Find the start of the next block.
 */
static inline void *get_next_block(void *block_start) {
  
  return block_start + get_block_size(block_start);
}

/*
 * Find the start of the previous block.
 */
static inline void *get_previous_block(void *block_start) {
  
  /* TO BE COMPLETED BY THE STUDENT. */
  
  void *prevFooter = block_start - HEADER_SIZE;
  int size = get_block_size(prevFooter);
  return block_start - size;
}

/*
 * Determine whether or not the given block is at the front of the
 * heap.
 */
static inline int is_first_block(heap *h, void *block_start) {
  
  return block_start == h->start;
}

/*
 * Determine whether or not the given address is inside the heap
 * region. Can be used to loop through all blocks:
 *
 * for (blk = h->start;
        is_within_heap_range(h, blk);
        blk = get_next_block(blk)) ...
 */
static inline int is_within_heap_range(heap *h, void *addr) {
  
  return addr >= h->start && addr < h->start + h->size;
}

/*
 * Coalesce a block with its consecutive block, but only if both
 * blocks are free. Returns a pointer to the beginning of the
 * coalesced block.
 */
static inline void *coalesce(heap *h, void *first_block_start) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  void *nextBlk = get_next_block(first_block_start);

  if (nextBlk != NULL && is_within_heap_range(h, nextBlk) && block_is_in_use(nextBlk) == 0
      && block_is_in_use(first_block_start) == 0) {
    block_size_t newBlkSize = get_block_size(first_block_start) + get_block_size(nextBlk);
    set_block_header(first_block_start, newBlkSize, 0);
  }

  //NOT SURE IF WE NEED TO RETURN NULL  
  return first_block_start;
}

/*
 * Determine the size of the block we need to allocate given the size
 * the user requested. Don't forget we need space for the header and
 * footer, and that the user size may not be aligned.
 */
static inline block_size_t get_size_to_allocate(block_size_t user_size) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  block_size_t size = user_size + 2*HEADER_SIZE; 
  if (user_size % PAYLOAD_ALIGN != 0) {
    size += PAYLOAD_ALIGN - user_size % PAYLOAD_ALIGN;
  } 

  return size;
}

/*
 * Turn a free block into one the user can utilize. Split the block if
 * it's more than twice as large or MAX_UNUSED_BYTES bytes larger than
 * needed. The real_size argument refers to the minimum block size
 * needed to handle a user requested size (i.e., the result of
 * get_size_to_allocate).
 */
static inline void *prepare_block_for_use(void *block_start, block_size_t real_size) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  block_size_t blkSize = get_block_size(block_start);

  if (blkSize > 2 * real_size || blkSize > real_size + MAX_UNUSED_BYTES) {
    void *nextBlk = block_start + real_size;
    set_block_header(block_start, real_size, 1);
    set_block_header(nextBlk, blkSize - real_size, 0);   
  } 
  else {
    set_block_header(block_start, blkSize, 1);
  }

  return get_payload(block_start);
}

/*
 * Create a heap that is "size" bytes large, including its header.
 */
heap *heap_create(intptr_t size, search_alg_t search_alg) {
  
  /* Allocate space in the process' actual heap */
  void *heap_start = sbrk(size);
  if (heap_start == (void *) -1) return NULL;
  
  /* Use the first part of the allocated space for the heap header */
  heap *h     = heap_start;
  heap_start += sizeof(heap);
  size       -= sizeof(heap);
  
  /* Ensures the heap_start points to an address that has space for
     the header, while allowing the payload to be aligned to PAYLOAD_ALIGN */
  int delta = PAYLOAD_ALIGN - HEADER_SIZE - ((uintptr_t) heap_start) % PAYLOAD_ALIGN;
  if (delta < 0)
    delta += PAYLOAD_ALIGN;
  if (delta > 0) {
    heap_start += delta;
    size -= delta;
  }
  
  /* Ensures the size points to as many bytes as necessary so that
     only full-sized blocks fit into the heap. */
  size -= (size - 2 * HEADER_SIZE) % PAYLOAD_ALIGN;
  
  h->size = size;
  h->start = heap_start;
  h->search_alg = search_alg;
  h->next = h->start;
  
  set_block_header(h->start, size, 0);
  return h;
}
/*
 * Print the structure of the heap to the screen.
 */
void heap_print(heap *h) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  if (h == NULL) {
    return;
  }
  
  if (h->search_alg == HEAP_FIRSTFIT) {
    printf("Algorithm: FIRST FIT\n");
  } else if (h->search_alg == HEAP_NEXTFIT) {
    printf("Algorithm: NEXT FIT\n");
  } else {
    printf("Algorithm: BEST FIT\n");
  }

  for (void *blk = h->start; is_within_heap_range(h, blk); blk = get_next_block(blk)){
    printf("Block at address: %p / Size: %d / ", (long*)blk, get_block_size(blk));
    if (block_is_in_use(blk) == 1) {
      printf("In use\n");
    } else {
      printf("Free\n");
    }
  }
}

/*
 * Determine the average size of all free blocks.
 */
block_size_t heap_find_avg_free_block_size(heap *h) {
  
  /* TO BE COMPLETED BY THE STUDENT. */
  
  long int totalSize = 0;
  long int count = 0;

  for (void *blk = h->start; is_within_heap_range(h, blk); blk = get_next_block(blk)) {
    if (block_is_in_use(blk) == 0) {
      totalSize += get_block_size(blk);
      count++;
    }
  }

  if (count != 0) {
    return (block_size_t) totalSize/count;
  }

  return 0;
}

/*
 * Free a block on the heap h. Beware of the case where the heap uses
 * a next fit search strategy, and h->next is pointing to a block that
 * is to be coalesced.
 */
void heap_free(heap *h, void *payload) {
  
  /* TO BE COMPLETED BY THE STUDENT. */
  
  void *blkStart = get_block_start(payload);
  void *prevBlk = get_previous_block(blkStart);
  void *nextBlk = get_next_block(blkStart);

  set_block_header(blkStart, get_block_size(blkStart), 0);
  
  if (h->search_alg == HEAP_NEXTFIT && nextBlk == h->next) {
    h->next = get_next_block(nextBlk);
    if (!is_within_heap_range(h, h->next)) {
      h->next = h->start;
    }
  }
  
  coalesce(h, blkStart);
  if (is_within_heap_range(h, prevBlk)) {
    coalesce(h, prevBlk);
  }  
}

/*
 * Malloc a block on the heap h, using first fit. Return NULL if no
 * block large enough to satisfy the request exists.
 */
static void *malloc_first_fit(heap *h, block_size_t user_size) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  block_size_t realSize = get_size_to_allocate(user_size);

  for (void *blk = h->start; is_within_heap_range(h, blk); blk = get_next_block(blk)) {
    block_size_t blkSize = get_block_size(blk);
    if (blkSize >= realSize && block_is_in_use(blk) == 0) {
      prepare_block_for_use(blk, realSize);    
      return get_payload(blk);
    }
  }
  
  return NULL;
}

/*
 * Malloc a block on the heap h, using best fit. Return NULL if no block
 * large enough to satisfy the request exists.
 */
static void *malloc_best_fit(heap *h, block_size_t user_size) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  void *minBlk = NULL;
  block_size_t minBlkSize = 0xFFFFFFFF;
  block_size_t realSize = get_size_to_allocate(user_size);
  
  for (void *blk = h->start; is_within_heap_range(h, blk); blk = get_next_block(blk)) {
    block_size_t blkSize = get_block_size(blk);    
    if (blkSize >= realSize && blkSize < minBlkSize && block_is_in_use(blk) == 0) {  
      minBlkSize = blkSize;
      minBlk = blk;
    }
  }
  if (minBlk != NULL) {
    prepare_block_for_use(minBlk, realSize); 
    return get_payload(minBlk);
  }
  
  return NULL;
}

/*
 * Malloc a block on the heap h, using next fit. Return NULL if no block
 * large enough to satisfy the request exists.
 */
static void *malloc_next_fit(heap *h, block_size_t user_size) {
  
  /* TO BE COMPLETED BY THE STUDENT. */

  block_size_t realSize = get_size_to_allocate(user_size);
  bool flag = false;

  for (void *blk = h->next; is_within_heap_range(h, blk); blk = get_next_block(blk)) {
    block_size_t blkSize = get_block_size(blk);
    if (blkSize >= realSize && block_is_in_use(blk) == 0) {
      prepare_block_for_use(blk, realSize);     
      h->next = blk;
      return get_payload(blk);
    } 
    if (h->start + h->size == get_next_block(blk) && !flag) {
      flag = true;
      blk = h->start;
      h->next = blk;
    }
  }

  return NULL;
}

/*
 * Our implementation of malloc.
 */
void *heap_malloc(heap *h, block_size_t size) {
  
  switch (h->search_alg) {
  case HEAP_FIRSTFIT:
    return malloc_first_fit(h, size);
  case HEAP_NEXTFIT:
    return malloc_next_fit(h, size);
  case HEAP_BESTFIT:
    return malloc_best_fit(h, size);
  }
  return NULL;
}
