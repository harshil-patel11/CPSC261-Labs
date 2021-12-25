#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "implicit.h"

/* Set to 1 for additional debugging statements. */
#define DEBUG 0

/* Default size for the heap is 2MB( 2^21 B). */
#define HEAP_SIZE (1 << 21)

/* Default size for array holding pointers returned by malloc. */
#define MAX_POINTERS 1000

/* Number of operations to perform in each heap implementation. */
#define OP_COUNT 50000

/*
 * Function to initialize the random number generator.
 */
void initialize_rng() {
  
  struct timeval t1;
  gettimeofday(&t1, NULL);
  srand(t1.tv_usec * t1.tv_sec);
}

/* Function to pick a rand size for a new block. */
unsigned long get_rand_block_size() {
  
  unsigned long size = 4;
  while (size < 512 && rand() % 6 != 0)
    size <<= 1;
  
  while (size < 2048 && rand() % 2 != 0)
    size <<= 1;
  
  return size + rand() % size;
}

/*
 * Main function.
 */
int main(int argc, char *argv[]) {
  
  initialize_rng();
  
  heap *h_first = heap_create(HEAP_SIZE, HEAP_FIRSTFIT);
  heap *h_next = heap_create(HEAP_SIZE, HEAP_NEXTFIT);
  heap *h_best = heap_create(HEAP_SIZE, HEAP_BESTFIT);
  char *pointer_array_first[MAX_POINTERS];
  char *pointer_array_next[MAX_POINTERS];
  char *pointer_array_best[MAX_POINTERS];
  unsigned long size, op_count = OP_COUNT;
  int nb_pointers = 0;
  int index;
  
  while (op_count-- > 0) {
    
    /* Randomly chooses between malloc and free. Always uses malloc if there
       is nothing to free. More allocated pointers means higher probability
       free will be chosen.
     */
    if ((nb_pointers == 0) || (rand() % MAX_POINTERS > nb_pointers)) {
      
      size = get_rand_block_size();
#if DEBUG
      printf("Malloc'ing %ld bytes for pointer %d.\n", size, nb_pointers);
#endif
      pointer_array_first[nb_pointers] = heap_malloc(h_first, size);
      if (pointer_array_first[nb_pointers] == NULL) {
	      printf("First fit: Ran out of memory with %ld operations left.\n", op_count);
      }

      pointer_array_next[nb_pointers] = heap_malloc(h_next, size);
      if (pointer_array_next[nb_pointers] == NULL) {
	      printf("Next fit: Ran out of memory with %ld operations left.\n", op_count);
      }

      pointer_array_best[nb_pointers] = heap_malloc(h_best, size);
      if (pointer_array_best[nb_pointers] == NULL) {
	      printf("Best fit: Ran out of memory with %ld operations left.\n", op_count);
      }

      nb_pointers++;
    }
    else {
      
      index = rand() % nb_pointers--;
#if DEBUG
      printf("Freeing pointer %d at addresses %p/%p/%p\n", index,
	     pointer_array_first[index],
	     pointer_array_next[index],
	     pointer_array_best[index]);
#endif
      if (pointer_array_first[index])
	      heap_free(h_first, pointer_array_first[index]);        
      if (pointer_array_next[index])
	      heap_free(h_next, pointer_array_next[index]);
      if (pointer_array_best[index])
	      heap_free(h_best, pointer_array_best[index]);
      
      pointer_array_first[index] = pointer_array_first[nb_pointers];
      pointer_array_next [index] = pointer_array_next [nb_pointers];
      pointer_array_best [index] = pointer_array_best [nb_pointers];
    }
#if DEBUG
    heap_print(h_first);
    heap_print(h_next);
    heap_print(h_best);
#endif
  }
  printf("First fit average block size: %u\n", heap_find_avg_free_block_size(h_first));
  printf("Next  fit average block size: %u\n", heap_find_avg_free_block_size(h_next));
  printf("Best  fit average block size: %u\n", heap_find_avg_free_block_size(h_best));
  return 0;
}

